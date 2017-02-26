#include "abstractmode.h"

AbstractMode::AbstractMode(Ui::MainWindow* mui, Joystick* mjoystick, Sensors* msensors, CCamera *mcam) :
    QObject(0), ui(mui), joystick(mjoystick), sensors(msensors), cam(mcam)
{

}

void AbstractMode::idle()
{
}
