#ifndef SENSORS_H
#define SENSORS_H

#include <QObject>
#include <QThread>
#include <QAtomicInt>

class Sensors : public QThread
{
    Q_OBJECT

public:
    Sensors();
    ~Sensors();

    QAtomicInt pingLeft;
    QAtomicInt pingRight;
    QAtomicInt pingFront;
    QAtomicInt gyro[3];
    QAtomicInt accel[3];
    QAtomicInt compass[3];
    QAtomicInt temperature;

    QAtomicInt gyroOffset[3];
    QAtomicInt accelOffset[3];

    QAtomicInt motorSpeed[4];    // -100 to 100
    QAtomicInt motorDistance[4];

protected:
    void run();

private:
    bool continueThread;
    int mpuOpen();
    void readGyro();
    void readAccel();
    void readCompass();
    void readTemp();
    void readProp();
    void reloadProp();
};

#endif // SENSORS_H
