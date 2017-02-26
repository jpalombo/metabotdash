#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <QThread>
#include <QFile>

class Joystick : public QThread
{
    Q_OBJECT

public:
    Joystick();
    ~Joystick();

    QAtomicInt posx;
    QAtomicInt posy;
    bool dmh;       // Dead Man's Handle

protected:
    void run();

private:
    void open();
    void decode(QByteArray event);

    bool continueThread;
    QFile port;

signals:
    void joystickConnected(bool);
    void update();
};

#endif // JOYSTICK_H
