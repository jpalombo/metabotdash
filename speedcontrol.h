#ifndef SPEEDCONTROL_H
#define SPEEDCONTROL_H

#include <QObject>
#include <QThread>
#include <QTime>
#include <QMutex>
#include "sensors.h"

class SpeedControl : public QThread
{
    Q_OBJECT

public:
    SpeedControl(Sensors*);
    ~SpeedControl();

    void setAccel(int);
    void setSpeed(int);
    void setDirection(int);
    void setSpeedDir(int, int);
    void stop();
    int accel();
    int direction();
    int speed();
    int stoppingDistance();

protected:
    void run();
    int update();

    QMutex lock;
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
    bool continueThread;
};

#endif // SPEEDCONTROL_H
