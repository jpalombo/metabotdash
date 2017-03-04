#include "manualmode.h"

ManualMode::ManualMode(Ui::MainWindow* mui, Joystick* mjoystick, Sensors* msensors, CCamera *mcam) :
    AbstractMode(mui, mjoystick, msensors, mcam)
{
    ui->pages->setCurrentWidget(ui->manualpage);
    connect(joystick, &Joystick::update, this, &ManualMode::joystickUpdate);
}

void ManualMode::joystickUpdate()
{
    ui->posx->setText(QString::number(joystick->posx));
    ui->posy->setText(QString::number(joystick->posy));
    ui->dmh->setText(QString::number(joystick->dmh));

    speed.setSpeedDir(joystick->posy/500, joystick->posx/1000);
}
