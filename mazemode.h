#ifndef MAZEMODE_H
#define MAZEMODE_H

#include <QFile>
#include "abstractmode.h"
#include "filter.h"
#include "pid.h"

class MazeMode : public AbstractMode
{
public:
    MazeMode(Ui::MainWindow *mui, Joystick *mjoystick, Sensors *msensors, CCamera *mcam);
    ~MazeMode();
    void idle();
    void go(bool checked);
    void buttonClicked(AbstractMode::Buttons button, bool checked);

private:
    QFile file;
    int lastdistance;
    Filter pingLeft;
    Filter pingRight;
    Filter pingFront;
    int state;

    void calcDirection();
    Pid pingpid;
    Pid bearingpid;

    enum Wall {NoWall, LeftWall, RightWall};
    int targetBearing;
    int targetWallClearance;
    Wall targetWall;

    void followWall(Wall wall, int clearance, int bearing);

public slots:
    void joystickUpdate(int, int, int);
};

#endif // MAZEMODE_H
