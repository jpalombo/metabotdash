#include "ui_mainwindow.h"
#include <QDebug>
#include <QTimer>
#include <wiringPi.h>
#include "modes.h"
#include "manualmode.h"
#include "speedmode.h"
#include "mazemode.h"
#include "linemode.h"
#include "statusmode.h"

Modes::Modes(Ui::MainWindow *mui, QObject *parent) :
    QObject(parent), ui(mui)
{
    // enable light pin and turn light off
    wiringPiSetupGpio();
    pinMode(27, OUTPUT);
    digitalWrite (27, LOW);

    //kick off the sensors
    sensors = new Sensors;
    joystick = new Joystick;
    connect(joystick, &Joystick::joystickConnected, this, &Modes::joystickConnected);

    //init camera
    ui->glWidget->update();
    cam = StartCamera(320, 240, 30, 1, true);
    buffWidth = 320;
    buffHeight = 240;
    ui->glWidget->setBuffSize(buffWidth, buffHeight);

    // set default mode
    mode = new ManualMode(ui, joystick, sensors, cam);

    // kick off idle timer
    timer0 = new QTimer(this);
    connect(timer0, SIGNAL(timeout()), this, SLOT(idle()));
    timer0->start();
}

Modes::~Modes()
{
    StopCamera();
}

void Modes::setMode(Modes::ModeType newMode)
{
    delete mode;

    switch (newMode) {
    case Manual:
        mode = new ManualMode(ui, joystick, sensors, cam);
        break;
    case Maze:
        mode = new MazeMode(ui, joystick, sensors, cam);
        break;
    case Speed:
        mode = new SpeedMode(ui, joystick, sensors, cam);
        break;
    case Line:
        mode = new LineMode(ui, joystick, sensors, cam);
        break;
    case Status:
        mode = new StatusMode(ui, joystick, sensors, cam);
        break;
    }
}

void Modes::joystickConnected(bool connected)
{
    if (connected) {
        ui->statusBar->showMessage("Joystick connected");
    } else {
        ui->statusBar->showMessage("Joystick not connected - try starting it");
    }

}

void Modes::idle()
{
    mode->idle();
}
