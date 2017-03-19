#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <QThread>
#include <QFile>

#define BUTTONS 14

/*  Playstation Joystick has the following controls
 *
 * Button
 * 0:   square
 * 1:   cross
 * 2:   circle
 * 3:   triangle
 * 4:   left trigger (L1)
 * 5:   right trigger (R1)
 * 6:   left trigger (L2)
 * 7:   right trigger (R2)
 * 8:   share
 * 9:   options
 * 10:  left joystick press
 * 11:  right joystick press
 * 12:  playstation
 *
 * Axis
 * 0:   left joystick, x axis
 * 1:   left joystick, y axis
 * 2:   right joystick, x axis
 * 3:   left trigger (L2)
 * 4:   right trigger (R2)
 * 5:   right joystick, y axis
 * 6:   left button pad, xaxis
 * 7:   left button pad, y axis
 * 8:   gyro x
 * 9:   gyro y
 * 10:  gyro z
 * 11:  force x
 * 12:  force y
 * 13:  force z
 */


class Joystick : public QThread
{
    Q_OBJECT

public:
    Joystick();
    ~Joystick();

    QAtomicInt posx;
    QAtomicInt posy;
    bool dmh;       // Dead Man's Handle
    bool button[BUTTONS];

protected:
    void run();

private:
    void open();
    void decode(QByteArray event);

    bool continueThread;
    QFile port;

signals:
    void joystickConnected(bool);
    void update(int, int, int);
};

#endif // JOYSTICK_H
