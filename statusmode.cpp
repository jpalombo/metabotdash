#include "statusmode.h"

StatusMode::StatusMode(Ui::MainWindow* mui, Joystick* mjoystick, Sensors* msensors, CCamera *mcam) :
    AbstractMode(mui, mjoystick, msensors, mcam)
{
    ui->pages->setCurrentWidget(ui->testpage);
    sensors->autoPing = true;
}

StatusMode::~StatusMode()
{
    sensors->autoPing = true;
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
}
