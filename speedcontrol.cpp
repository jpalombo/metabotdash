#include "speedcontrol.h"

SpeedControl::SpeedControl(Sensors * s) :
    sensors(s), targetAccel(0)
{
    stop();
}

void SpeedControl::setAccel(int a)
{
    targetAccel = a;
    setSpeedDir(targetSpeed, targetDirection);
}

void SpeedControl::setSpeed(int s)
{
    setSpeedDir(s, targetDirection);
}

void SpeedControl::setDirection(int d)
{
    targetDirection = d;
    targetSpeedL = targetSpeed - targetDirection;
    targetSpeedR = targetSpeed + targetDirection;
}

void SpeedControl::stop()
{
    setSpeedDir(0, 0);
}

int SpeedControl::setSpeedDir(int s, int d)
{
    targetSpeed = s;
    targetDirection = d;

    startSpeedL = sensors->motorSpeed[0];
    startSpeedR = sensors->motorSpeed[1];
    targetSpeedL = targetSpeed - targetDirection;
    targetSpeedR = targetSpeed + targetDirection;

    if (targetAccel == 0) {
        accelms = 0;
    } else {
        int deltaL = qAbs(targetSpeedL - startSpeedL);
        int deltaR = qAbs(targetSpeedR - startSpeedR);
        int maxdelta = qMax(deltaL, deltaR);
        accelms = maxdelta * 1000 / targetAccel;
    }
    timer.start();
    return update();
}

int SpeedControl::update()
{
    int leftspeed;
    int rightspeed;

    if (timer.elapsed() >= accelms) {
        leftspeed = targetSpeedL;
        rightspeed = targetSpeedR;
    } else {
        leftspeed = startSpeedL + ((targetSpeedL - startSpeedL) * timer.elapsed() / accelms);
        rightspeed = startSpeedR + ((targetSpeedR - startSpeedR) * timer.elapsed() / accelms);
    }

    sensors->motorSpeed[0] = leftspeed;
    sensors->motorSpeed[3] = leftspeed;
    sensors->motorSpeed[1] = rightspeed;
    sensors->motorSpeed[2] = rightspeed;
    return (leftspeed + rightspeed)/2;
}

int SpeedControl::accel()
{
   return targetAccel;
}

int SpeedControl::direction()
{
    return targetDirection;
}

int SpeedControl::speed()
{
    return targetSpeed;
}

