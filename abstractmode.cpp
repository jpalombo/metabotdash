#include "abstractmode.h"

AbstractMode::AbstractMode(Ui::MainWindow* mui, Joystick* mjoystick, Sensors* msensors, CCamera *mcam) :
    QObject(0), ui(mui), joystick(mjoystick), sensors(msensors), cam(mcam), speed(msensors)
{

}

void AbstractMode::idle()
{
}

void AbstractMode::go(bool)
{

}

void AbstractMode::buttonClicked(AbstractMode::Buttons button, bool checked)
{
    Q_UNUSED(button);
    Q_UNUSED(checked);
}

