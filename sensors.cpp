#include <QDebug>
#include <QProcess>

#include "sensors.h"
extern "C" {
#include "mpu/inv_mpu.h"
#include "mpu/I2Cdev.h"
}

Sensors::Sensors() :
    gyroOffset{0, 0, 0},
    accelOffset{0, 0, 0},
    autoPing(false),
    continueThread(true)
{
    for(int i = 0; i < 4; i++) {
        motorSpeed[i] = 0;
    }

    reloadProp();
    start();
}

Sensors::~Sensors()
{
    continueThread = false;
    exit();
    wait();
}

void Sensors::reloadProp()
{
    QProcess proploader;
    qDebug() << "Reloading Propeller...";
    proploader.start("propman /home/pi/metabot3/spin/mb3.binary");
    proploader.waitForFinished();
    qDebug() << QString::fromLatin1(proploader.readAllStandardError());
    sleep(1);
}

void Sensors::run()
{
    mpuOpen();
    while (continueThread) {
        msleep(2);
        readGyro();
        readAccel();
        readCompass();
        readTemp();
        readProp();
    }
}

int Sensors::mpuOpen()
{
    unsigned char devStatus; // return status after each device operation

    // initialize device
    qDebug() << "Initializing MPU...";
    if (mpu_init(NULL) != 0) {
        qDebug() << "MPU init failed!";
        return -1;
    }
    qDebug() << "Setting MPU sensors...";
    if (mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL | INV_XYZ_COMPASS) != 0) {
        qDebug() << "Failed to set sensors!";
        return -1;
    }
    qDebug() << "Setting GYRO sensitivity...";
    if (mpu_set_gyro_fsr(250) != 0) {
        qDebug() << "Failed to set gyro sensitivity!";
        return -1;
    }
    qDebug() << "Setting ACCEL sensitivity...";
    if (mpu_set_accel_fsr(2) != 0) {
        qDebug() << "Failed to set accel sensitivity!";
        return -1;
    }
  qDebug() << "Setting sample rate...";
  if (mpu_set_sample_rate(500) != 0){
    qDebug() << "Failed to set sample rate!";
    return -1;
  }
  qDebug() << "Setting Compass sample rate...";
    if (mpu_set_compass_sample_rate(10) != 0) {
        qDebug() << "Failed to set compass sample rate!";
        return -1;
    }
    // verify connection
    qDebug() << "Powering up MPU...";
    mpu_get_power_state(&devStatus);
    if (devStatus == 1)
        qDebug() << "MPU6050 connection successful";
    else
        qDebug() << "MPU6050 connection failed" << devStatus;

    // calibrating
    short m_gyro[3];
    int gyrosum[3] = { 0, 0, 0 };
    int count = 0;
    int i;
    qDebug() << "Calibrating...";
    while (count < 300) {
        if (mpu_get_gyro_reg(m_gyro, 0) == 0) {
            for (i = 0; i < 3; i++)
                gyrosum[i] += m_gyro[i];
            count++;
        }
        usleep(1000);
    }
    for (i = 0; i < 3; i++)
        gyroOffset[i] = gyrosum[i] / count;
    qDebug() << "Gyro offset :" << gyroOffset[0] << gyroOffset[1] << gyroOffset[2];
    qDebug() << "Done.";
    return 0;
}

void Sensors::readGyro() {
    short m_gyro[3];        // [x, y, z]           gyro vector
    if (mpu_get_gyro_reg(m_gyro, 0) == 0) {
        gyro[0] = m_gyro[0] - gyroOffset[0];
        gyro[1] = m_gyro[1] - gyroOffset[1];
        gyro[2] = m_gyro[2] - gyroOffset[2];
    } else {
        qDebug() << "Failed to read gyro";
    }
}

void Sensors::readAccel() {
    short m_accel[3];       // [x, y, z]           accel vector
    if (mpu_get_accel_reg(m_accel, 0) == 0) {
        accel[0] = m_accel[0];
        accel[1] = m_accel[1];
        accel[2] = m_accel[2];
    } else {
        qDebug() << "Failed to read accel";
    }
}

void Sensors::readCompass() {
    short m_compass[3];
    int rc = mpu_get_compass_reg(m_compass, 0);
    if (rc == 0) {
        compass[0] = m_compass[0];
        compass[1] = m_compass[1];
        compass[2] = m_compass[2];
    } else {
        qDebug() << "Failed to read compass" << rc;
    }
}

void Sensors::readTemp()
{
    long temp;
    if (mpu_get_temperature(&temp, 0) == 0){
        temperature = temp;
    } else {
       qDebug() << "Failed to read temperature";
    }
}

// I2C interface : registers
//      0 :     Motor 0 position (4 bytes)
//      4 :     Motor 1 position (4 bytes)
//      8 :     Motor 2 position (4 bytes)
//      12 :    Motor 3 position (4 bytes)
//      16 :    Left Ping distance in mm (2 bytes)
//      18 :    Right Ping distance in mm (2 bytes)
//      20 :    Front Ping distance in mm (2 bytes)
//      22 :    Target Motor 0 Speed (1 byte) signed -127 - 127
//      23 :    Target Motor 1 Speed (1 byte)
//      24 :    Target Motor 2 Speed (1 byte)
//      25 :    Target Motor 3 Speed (1 byte)
//      26 :    Options (low bit = autoping on / off)
//      27 :    Read Ready (master sets to 1 when ready to read, slave sets to zero when multi-byte values updated
void Sensors::readProp()
{
    // Write out speed registers and autoping setting
    QByteArray registers(32, 0);
    for(int i = 0; i < 4; i++) {
        int8_t s = motorSpeed[i];
        registers[i] = s;
    }
    registers[4] = autoPing;
    writeBytes(0x42, 22, 5, (uint8_t *) registers.data());

    // Check if there is data ready to read
    int count = readBytes(0x42, 27, 1, (uint8_t *)registers.data());  // Read Register 27 = Read Ready
    if (count < 0) {
        // Propeller has reset itself, try reloading
        reloadProp();
        return;
    }
    if ((count > 0) && (registers.at(0) == 0)) {  // Check Read Ready flag
        int totalD = 0;
        // read the data
        count = readBytes(0x42, 0, 22, (uint8_t *)registers.data());
        for(int i = 0; i < 4; i++) {
            int d = registers.at(i*4);
            for(int j = 1; j < 4; j++) {
                d += registers.at(i*4 + j) << (j*8);
            }
            motorDistance[i] = d;
            totalD += d;
        }
        totalDistance = totalD;
        pingLeft = registers.at(16) + (registers.at(17) << 8);
        pingRight = registers.at(18) + (registers.at(19) << 8);
        pingFront = registers.at(20) + (registers.at(21) << 8);
        writeByte(0x42, 27, 1);  // prime ready for next read
    }
}
