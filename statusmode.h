#ifndef STATUSMODE_H
#define STATUSMODE_H

#include "abstractmode.h"

class StatusMode : public AbstractMode
{
public:
    StatusMode(Ui::MainWindow *mui, Joystick *mjoystick, Sensors *msensors, CCamera *mcam);
    ~StatusMode();
    void idle();
};

#endif // STATUSMODE_H
