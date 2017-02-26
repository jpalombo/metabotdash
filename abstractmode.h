#ifndef ABSTRACTMODE_H
#define ABSTRACTMODE_H

#include <QObject>
#include "ui_mainwindow.h"
#include "joystick.h"
#include "sensors.h"
#include "camera.h"

namespace Ui {
class MainWindow;
}

class AbstractMode : public QObject
{
    Q_OBJECT
public:
    explicit AbstractMode(Ui::MainWindow*, Joystick*, Sensors*, CCamera*);
    virtual void idle();  // called when the message loop is idle

protected:
    Ui::MainWindow *ui;
    Joystick *joystick;
    Sensors *sensors;
    CCamera* cam;

signals:

public slots:
};

#endif // ABSTRACTMODE_H
