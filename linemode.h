#ifndef LINEMODE_H
#define LINEMODE_H

#include <QTimer>
#include "abstractmode.h"

class LineMode : public AbstractMode
{
    Q_OBJECT
public:
    LineMode(Ui::MainWindow *mui, Joystick *mjoystick, Sensors *msensors, CCamera *mcam);
    ~LineMode();
    void idle();

private:
    void setLightsOn(bool lightsOn);
    QTimer *timer1sec;
    int fps;

public slots:
    void capture();
    void onesec();
};

#endif // LINEMODE_H
