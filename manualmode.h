#ifndef MANUALMODE_H
#define MANUALMODE_H

#include "abstractmode.h"

class ManualMode : public AbstractMode
{
public:
    ManualMode(Ui::MainWindow *mui, Joystick *mjoystick, Sensors *msensors, CCamera *mcam);
    ~ManualMode();
    void idle();

public slots:
    void joystickUpdate(int, int, int);
};

#endif // MANUALMODE_H
