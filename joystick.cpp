#include <QDebug>
#include <QByteArray>

#include "joystick.h"

Joystick::Joystick() :
    posx(0), posy(0), dmh(false), continueThread(true)
{
    start();
}


Joystick::~Joystick()
{
    continueThread = false;
    exit();
    wait();
}

void Joystick::run()
{
    while (continueThread) {
        if (!port.isOpen())
           open();

        if (port.isOpen()) {
            QByteArray data = port.read(8);
            if (data.length() == 8)
                decode(data);
             msleep(1);
        } else {
            msleep(1000);    // Wait one second before retrying open
        }
    }
}

void Joystick::decode(QByteArray event)
{
    // Packet format defined here : https://www.kernel.org/doc/Documentation/input/joystick-api.txt
    int eType = event[6];
    int eNumber = event[7];
    int16_t eValue = uint16_t(event[4]) | uint16_t(event[5])<<8;

    if (eType == 1) {  //Button
        if (eNumber < BUTTONS && button[eNumber] != (eValue != 0)) {
            button[eNumber] = (eValue != 0);
            emit update(eType, eNumber, eValue);
        }
    } else if (eType == 2) {  //Axis
        if (eNumber == 2 && posx != eValue) {
            posx = eValue;
            emit update(eType, eNumber, eValue);
        }
        else if (eNumber == 5 && posy != eValue) {
            posy = eValue;
            emit update(eType, eNumber, eValue);
        }
        else if ((eNumber == 3 || eNumber == 4) && (dmh != (eValue > 0))) {
            dmh = (eValue > 0);
            emit update(eType, eNumber, eValue);
        }
    }
}

void Joystick::open()
{
    port.close();
    port.setFileName("/dev/input/js0");
    port.open(QIODevice::ReadOnly | QIODevice::Unbuffered);
    emit joystickConnected (port.isOpen());
}

