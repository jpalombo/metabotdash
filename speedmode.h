#ifndef SPEEDMODE_H
#define SPEEDMODE_H

#include <QTime>
#include <QTimer>
#include "abstractmode.h"
#include "pid.h"

class SpeedMode : public AbstractMode
{
public:
    SpeedMode(Ui::MainWindow *mui, Joystick *mjoystick, Sensors *msensors, CCamera *mcam);
    ~SpeedMode();
    virtual void go(bool);
    virtual void idle();

private:
    QTime timer;
    QTimer *timer1sec;
    Pid pid;
    int loopcount;
    int lastdistance;

    int calcDirection();

public slots:
    void joystickUpdate();
    void onesec();

};

#endif // SPEEDMODE_H
