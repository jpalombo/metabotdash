#include "manualmode.h"

ManualMode::ManualMode(Ui::MainWindow* mui, Joystick* mjoystick, Sensors* msensors, CCamera *mcam) :
    AbstractMode(mui, mjoystick, msensors, mcam)
{
    ui->pages->setCurrentWidget(ui->manualpage);
    connect(joystick, &Joystick::update, this, &ManualMode::joystickUpdate);
    speed.setAccel(0);
}

void ManualMode::joystickUpdate(int eType, int eNumber, int eValue)
{
    ui->posx->setText(QString::number(joystick->posx));
    ui->posy->setText(QString::number(joystick->posy));
    ui->dmh->setText(QString::number(joystick->dmh));

    speed.setSpeedDir(-joystick->posy/500, -joystick->posx/1000);

    if (eType == 1) {
        switch (eNumber) {
        case 0:
            ui->manualbutton0->setChecked(eValue != 0);
            break;
        case 1:
            ui->manualbutton1->setChecked(eValue != 0);
            break;
        case 2:
            ui->manualbutton2->setChecked(eValue != 0);
            break;
        case 3:
            ui->manualbutton3->setChecked(eValue != 0);
            break;
        case 4:
            ui->manualbutton4->setChecked(eValue != 0);
            break;
        case 5:
            ui->manualbutton5->setChecked(eValue != 0);
            break;
        case 6:
            ui->manualbutton6->setChecked(eValue != 0);
            break;
        case 7:
            ui->manualbutton7->setChecked(eValue != 0);
            break;
        case 8:
            ui->manualbutton8->setChecked(eValue != 0);
            break;
        case 9:
            ui->manualbutton9->setChecked(eValue != 0);
            break;
        case 10:
            ui->manualbutton10->setChecked(eValue != 0);
            break;
        case 11:
            ui->manualbutton11->setChecked(eValue != 0);
            break;
        }
    }
}

void ManualMode::idle()
{
}
