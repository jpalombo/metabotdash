#ifndef JOKEMODE_H
#define JOKEMODE_H

#include "abstractmode.h"
#include <QStringList>

class JokeMode : public AbstractMode
{
public:
    JokeMode(Ui::MainWindow *mui, Joystick *mjoystick, Sensors *msensors, CCamera *mcam);
    void buttonClicked(AbstractMode::Buttons button, bool checked);

private:
    QStringList jokes;
    QString answer;

public slots:
    void joystickUpdate(int, int, int);
    void punchline();

};

#endif // JOKEMODE_H
