#ifndef SPEEDCONTROL_H
#define SPEEDCONTROL_H

#include <QTime>
#include "sensors.h"

class SpeedControl
{
public:
    SpeedControl(Sensors*);

    void setAccel(int);
    void setSpeed(int);
    void setDirection(int);
    int setSpeedDir(int, int);
    void stop();
    int update();
    int accel();
    int direction();
    int speed();


private:
    Sensors *sensors;

    int targetSpeed;
    int targetDirection;
    QTime timer;
    int targetAccel;
    int startSpeedL;
    int startSpeedR;
    int targetSpeedL;
    int targetSpeedR;
    int accelms;

};

#endif // SPEEDCONTROL_H
