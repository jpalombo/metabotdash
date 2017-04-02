#ifndef SPEEDMODE_H
#define SPEEDMODE_H

#include <QTime>
#include <QTimer>
#include <QFile>
#include "abstractmode.h"
#include "filter.h"
#include "pid.h"

class SpeedMode : public AbstractMode
{
public:
    SpeedMode(Ui::MainWindow *mui, Joystick *mjoystick, Sensors *msensors, CCamera *mcam);
    ~SpeedMode();
    virtual void go(bool);
    virtual void idle();
    void buttonClicked(AbstractMode::Buttons button, bool checked);

private:
    QTime timer;
    QTimer *timer1sec;
    Filter pingLeft;
    Filter pingRight;
    int loopcount;
    int startdistance;
    QFile file;
    Pid bearingPid;

    int calcDirection();

public slots:
    void joystickUpdate(int, int, int);
    void onesec();

};

#endif // SPEEDMODE_H
