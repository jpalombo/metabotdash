#include "jokemode.h"
#include <QFile>
#include <QTextStream>
#include <QTimer>

JokeMode::JokeMode(Ui::MainWindow *mui, Joystick *mjoystick, Sensors *msensors, CCamera *mcam) :
    AbstractMode(mui, mjoystick, msensors, mcam)
{
    ui->pages->setCurrentWidget(ui->jokepage);
    speed.setAccel(0);
    speed.stop();

    jokes.clear();
    QFile file("/home/pi/metabot3/jokes.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    QTextStream in(&file);
    while (!in.atEnd()) {
        jokes.append(in.readLine());
    }
    file.close();
}

void JokeMode::buttonClicked(AbstractMode::Buttons button, bool checked)
{
    Q_UNUSED(checked);

    if (button != Joke)
        return;

    if (jokes.size() < 2)
        return;

    int index = rand() % jokes.size();
    QString joke = jokes[index];
    if (joke[0] == 'A') {
        index = (index + 1) % jokes.size();
        joke = jokes[index];
    }

    if (joke[0] == 'Q') {
        answer = jokes[index+1];
        answer.remove(0,2);
        QTimer::singleShot(3000, this, &JokeMode::punchline);
    }

    joke.remove(0,2);
    ui->joke1->setText(joke);
    ui->joke2->setText("");
}

void JokeMode::punchline()
{
    ui->joke2->setText(answer);
}
