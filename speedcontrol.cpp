#include <QMutexLocker>
#include "speedcontrol.h"

SpeedControl::SpeedControl(Sensors * s) :
    lock(), sensors(s), targetAccel(0), continueThread(true)
{
    stop();     // stop the robot
    start();    // start the update thread
}

SpeedControl::~SpeedControl()
{
    continueThread = false;
    exit();
    wait();

    sensors->motorSpeed[0] = 0;
    sensors->motorSpeed[3] = 0;
    sensors->motorSpeed[1] = 0;
    sensors->motorSpeed[2] = 0;
}

void SpeedControl::run()
{
    while (continueThread) {
        update();
        msleep(1);
    }
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
    setSpeedDir(targetSpeed, d);
}

void SpeedControl::stop()
{
    setSpeedDir(0, 0);
}

void SpeedControl::setSpeedDir(int s, int d)
{
    if (targetSpeed == s && targetDirection == d)
        return;  // nothing to do
    QMutexLocker locker(&lock);
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
    return;
}

int SpeedControl::update()
{
    QMutexLocker locker(&lock);
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
    QMutexLocker locker(&lock);
    return (sensors->motorSpeed[1] - sensors->motorSpeed[0]) / 2;
}

int SpeedControl::speed()
{
    QMutexLocker locker(&lock);
    return (sensors->motorSpeed[0] + sensors->motorSpeed[1]) / 2;
}

int SpeedControl::stoppingDistance()
{
    int v = speed();
    return 30*v*v/(targetAccel * 2);
}
