#include <QDebug>
#include "pid.h"

Pid::Pid(int Kp, int Ki, int Kd ) :
    mKp(Kp), mKi(Ki), mKd(Kd), integral(0), lasterr(0)
{

}

// assume that update is called at regular intervals, so ignore time
int Pid::update(int err)
{
    int delta = err - lasterr;
    lasterr = err;
    integral += err;

    //qDebug() << mKp * err <<  mKi * integral << mKd * delta;
    return (mKp * err) + (mKi * integral) + (mKd * delta);
}

void Pid::reset()
{
    integral = 0;
    lasterr = 0;
}

int Pid::kp()
{
    return mKp;
}

int Pid::ki()
{
    return mKi;
}

int Pid::kd()
{
    return mKd;
}

void Pid::setKp(int p)
{
    mKp = p;
}

void Pid::setKi(int i)
{
    mKi = i;
}

void Pid::setKd(int d)
{
    mKd = d;
}

