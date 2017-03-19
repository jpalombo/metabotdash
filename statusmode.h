#ifndef STATUSMODE_H
#define STATUSMODE_H

#include <QFile>
#include "abstractmode.h"

class StatusMode : public AbstractMode
{
public:
    StatusMode(Ui::MainWindow *mui, Joystick *mjoystick, Sensors *msensors, CCamera *mcam);
    ~StatusMode();
    void idle();
    void buttonClicked(AbstractMode::Buttons, bool checked);

private:
    bool configuring;
    int minread[3];
    int maxread[3];
    QFile file;

};

#endif // STATUSMODE_H
