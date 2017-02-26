#ifndef MAZEMODE_H
#define MAZEMODE_H

#include "abstractmode.h"

class MazeMode : public AbstractMode
{
public:
    MazeMode(Ui::MainWindow *mui, Joystick *mjoystick, Sensors *msensors, CCamera *mcam);
};

#endif // MAZEMODE_H
