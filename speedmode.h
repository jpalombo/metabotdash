#ifndef SPEEDMODE_H
#define SPEEDMODE_H

#include "abstractmode.h"

class SpeedMode : public AbstractMode
{
public:
    SpeedMode(Ui::MainWindow *mui, Joystick *mjoystick, Sensors *msensors, CCamera *mcam);
};

#endif // SPEEDMODE_H
