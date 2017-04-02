#include <QDebug>
#include <QProcess>
#include <QSettings>
#include <QCoreApplication>

#include "sensors.h"
extern "C" {
#include "mpu/inv_mpu.h"
#include "mpu/I2Cdev.h"
}

Sensors::Sensors() :
    gyroOffset{0, 0, 0},
    accelOffset{0, 0, 0},
    autoPing(0),
    servo{0, 0, 0},
    continueThread(true),
    paused(false)
{
    for(int i = 0; i < 4; i++) {
        motorSpeed[i] = 0;
    }
    QSettings settings;
    settings.beginGroup("Sensors");
    compassOffset[0] = settings.value("CompassOffsetX", 0).toInt();
    compassOffset[1] = settings.value("CompassOffsetY", 0).toInt();
    compassOffset[2] = settings.value("CompassOffsetZ", 0).toInt();
    compassScale[0]= settings.value("CompassScaleX", 0.0).toFloat();
    compassScale[1]= settings.value("CompassScaleY", 0.0).toFloat();
    compassScale[2]= settings.value("CompassScaleZ", 0.0).toFloat();
    settings.endGroup();

    reloadProp();
    start();
}

Sensors::~Sensors()
{
    QSettings settings;
    settings.beginGroup("Sensors");
    settings.setValue("CompassOffsetX", (int)compassOffset[0]);
    settings.setValue("CompassOffsetY", (int)compassOffset[1]);
    settings.setValue("CompassOffsetZ", (int)compassOffset[2]);
    settings.setValue("CompassScaleX", compassScale[0]);
    settings.setValue("CompassScaleY", compassScale[1]);
    settings.setValue("CompassScaleZ", compassScale[2]);
    settings.endGroup();

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
        if (!paused){
            readGyro();
            readAccel();
            readCompass();
            readTemp();
            readProp();
            integral += gyro[0];
            bearing = integral/1361;
        }
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
    qDebug() << "Calibrating...";
    msleep(50);
    configGyro(500);
    qDebug() << "Done.";
    return 0;
}

void Sensors::configGyro(int limit) {
    int count = 0;
    short m_gyro[3];
    int gyrosum[3] = { 0, 0, 0 };
    int i;

    paused = true;
    while (count < limit) {
        if (count % 16 == 0)
            QCoreApplication::processEvents();
        else
            msleep(1);
        if (mpu_get_gyro_reg(m_gyro, 0) == 0) {
            for (i = 0; i < 3; i++)
                gyrosum[i] += m_gyro[i];
            count++;
        }
    }
    for (i = 0; i < 3; i++)
        gyroOffset[i] = gyrosum[i] / count;
    bearing = 0;
    integral = 0;
    qDebug() << "Gyro offset :" << gyroOffset[0] << gyroOffset[1] << gyroOffset[2];
    paused = false;
}

void Sensors::resetBearing()
{
    integral = 0;
}

void Sensors::readGyro() {
    short m_gyro[3];        // [x, y, z]           gyro vector
    if (mpu_get_gyro_reg(m_gyro, 0) == 0) {
        for (int i = 0; i < 3; i++)
            gyro[i] = m_gyro[i] - gyroOffset[i];
    } else {
        qDebug() << "Failed to read gyro";
    }
}

void Sensors::readAccel() {
    short m_accel[3];       // [x, y, z]           accel vector
    if (mpu_get_accel_reg(m_accel, 0) == 0) {
        for (int i = 0; i < 3; i++)
            accel[i] = m_accel[i];
    } else {
        qDebug() << "Failed to read accel";
    }
}

void Sensors::readCompass() {
    short m_compass[3];
    int rc = mpu_get_compass_reg(m_compass, 0);
    if (rc == 0) {
        for (int i = 0; i < 3; i++)
            compass[i] = (int) ((float)(m_compass[i] - compassOffset[i]) * compassScale[i]);
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
//      27, 28 : Ball Thrower Motor Speed
//      29 :    Ball Thrower Servo 1
//      30 :    Ball Thrower Servo 2
//      31 :    Read Ready (master sets to 1 when ready to read, slave sets to zero when multi-byte values updated


void Sensors::readProp()
{
    // Write out speed registers, autoping setting and servo values
    QByteArray registers(32, 0);
    for(int i = 0; i < 4; i++) {
        int8_t s = -motorSpeed[i];
        registers[i] = s;
    }
    registers[4] = autoPing;
    registers[5] = (servo[0] >> 8) & 0xff;
    registers[6] = servo[0] & 0xff;
    registers[7] = servo[1] & 0xff;
    registers[8] = servo[2] & 0xff;
    writeBytes(0x42, 22, 9, (uint8_t *) registers.data());

    // Check if there is data ready to read
    int count = readBytes(0x42, 31, 1, (uint8_t *)registers.data());  // Read Register 31 = Read Ready
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
            motorDistance[i] = -d;
            totalD += -d;
        }
        totalDistance = totalD;
        pingLeft = registers.at(18) + (registers.at(19) << 8);
        pingRight = registers.at(16) + (registers.at(17) << 8);
        pingFront = registers.at(20) + (registers.at(21) << 8);
        writeByte(0x42, 31, 1);  // prime ready for next read
    }
}
