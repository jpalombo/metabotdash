#include <QTextStream>
#include "statusmode.h"

StatusMode::StatusMode(Ui::MainWindow* mui, Joystick* mjoystick, Sensors* msensors, CCamera *mcam) :
    AbstractMode(mui, mjoystick, msensors, mcam), configuring(false)
{
    ui->pages->setCurrentWidget(ui->testpage);
    sensors->autoPing = 2;  // left, right and front
    speed.stop();

    file.setFileName("/home/pi/metabot3/compassconfig.csv");
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);
    out << "gyro bearing, x, y, z\n";
}

StatusMode::~StatusMode()
{
    file.close();
    sensors->autoPing = 0;
}

void StatusMode::idle()
{
    ui->accelx->setText(QString::number(sensors->accel[0]));
    ui->accely->setText(QString::number(sensors->accel[1]));
    ui->accelz->setText(QString::number(sensors->accel[2]));
    ui->gyrox->setText(QString::number(sensors->gyro[0]));
    ui->gyroy->setText(QString::number(sensors->gyro[1]));
    ui->gyroz->setText(QString::number(sensors->gyro[2]));
    ui->compassx->setText(QString::number(sensors->compass[0]));
    ui->compassy->setText(QString::number(sensors->compass[1]));
    ui->compassz->setText(QString::number(sensors->compass[2]));
    ui->pingLeft->setText(QString::number(sensors->pingLeft));
    ui->pingRight->setText(QString::number(sensors->pingRight));
    ui->pingFront->setText(QString::number(sensors->pingFront));
    ui->statuspos0->setText(QString::number(sensors->motorDistance[0]));
    ui->statuspos1->setText(QString::number(sensors->motorDistance[1]));
    ui->statuspos2->setText(QString::number(sensors->motorDistance[2]));
    ui->statuspos3->setText(QString::number(sensors->motorDistance[3]));
    ui->bearing->setText(QString::number((double)(sensors->bearing) / 10.0, 'f', 1));
    ui->temperature->setText(QString::number(sensors->temperature));

    if (configuring && sensors->compass[0] != 10000) {
        for (int i = 0; i < 3; i++){
            if (minread[i] > sensors->compass[i])
                minread[i] = sensors->compass[i];
            if (maxread[i] < sensors->compass[i])
                maxread[i] = sensors->compass[i];
        }
        QTextStream out(&file);
        out << sensors->bearing
            << ", " << sensors->compass[0]
            << ", " << sensors->compass[1]
            << ", " << sensors->compass[2]
            << "\n";
    }
}

void StatusMode::buttonClicked(AbstractMode::Buttons button, bool checked)
{
    if (button == ConfigCompass) {
        configuring = checked;

        if (configuring) {
            ui->buildlabel->setText("Configuring Compass - Revolve through 360 degrees...");
            // Reset current offsets and scales to get raw readings
            for (int i = 0; i < 3; i++){
                sensors->compassOffset[i] = 0;
                sensors->compassScale[i] = 1.0;
                sensors->compass[i] = 10000;
            }
            for (int i = 0; i < 3; i++){
                minread[i] = 10000;
                maxread[i] = -10000;
            }

        } else {
            ui->buildlabel->setText("");
            qDebug() << "Compass Offsets and Scales :";
            for (int i = 0; i < 3; i++){
                sensors->compassOffset[i] = (maxread[i] + minread[i]) / 2;
                sensors->compassScale[i] = 200.0 / (float)(maxread[i] - minread[i]);
                qDebug() << i << sensors->compassOffset[i] << sensors->compassScale[i];
            }
        }
    } else if (button == ConfigGyro && checked) {
        sensors->configGyro(2000);
        ui->configGyro->setChecked(false);
    }
}
