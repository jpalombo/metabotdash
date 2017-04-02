#ifndef MODES_H
#define MODES_H

#include <QObject>
#include <QTimer>
#include "camera.h"
#include "sensors.h"
#include "joystick.h"
#include "abstractmode.h"

namespace Ui {
class MainWindow;
}

class Modes : public QObject
{
    Q_OBJECT
public:
    explicit Modes(Ui::MainWindow *mui, QObject *parent = 0);
    ~Modes();

    enum ModeType {Manual, Maze, Speed, Line, Status, April1};
    void setMode(ModeType);
    void buttonClicked(AbstractMode::Buttons, bool checked);

private:
    Ui::MainWindow *ui;
    CCamera* cam;
    QTimer *timer0;
    int buffWidth;
    int buffHeight;
    Sensors *sensors;
    Joystick *joystick;
    void setLightsOn(bool);
    AbstractMode *mode;

signals:

public slots:
    void joystickConnected(bool);
    void idle();

};

#endif // MODES_H
