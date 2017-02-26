#ifndef MANUALMODE_H
#define MANUALMODE_H

#include "abstractmode.h"

class ManualMode : public AbstractMode
{
public:
    ManualMode(Ui::MainWindow *mui, Joystick *mjoystick, Sensors *msensors, CCamera *mcam);

public slots:
    void joystickUpdate();
};

#endif // MANUALMODE_H
