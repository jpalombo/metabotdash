#ifndef ABSTRACTMODE_H
#define ABSTRACTMODE_H

#include <QObject>
#include "ui_mainwindow.h"
#include "joystick.h"
#include "sensors.h"
#include "camera.h"
#include "speedcontrol.h"

namespace Ui {
class MainWindow;
}

//#define CAMWIDTH 768
//#define CAMHEIGHT 512

#define CAMWIDTH 96
#define CAMHEIGHT 64

class AbstractMode : public QObject
{
    Q_OBJECT
public:
    enum Buttons {Go, ConfigCompass, ConfigGyro};
    explicit AbstractMode(Ui::MainWindow*, Joystick*, Sensors*, CCamera*);
    virtual void idle();        // called when the message loop is idle
    virtual void go(bool);
    virtual void buttonClicked(Buttons, bool checked);

protected:
    Ui::MainWindow *ui;
    Joystick *joystick;
    Sensors *sensors;
    CCamera* cam;
    SpeedControl speed;

};

#endif // ABSTRACTMODE_H
