#include <QSettings>
#include "speedmode.h"

SpeedMode::SpeedMode(Ui::MainWindow* mui, Joystick* mjoystick, Sensors* msensors, CCamera *mcam) :
    AbstractMode(mui, mjoystick, msensors, mcam), pid(1,0,0), loopcount(0)
{
    ui->pages->setCurrentWidget(ui->speedpage);

    QSettings settings;
    settings.beginGroup("SpeedMode");
    ui->speedmax->setValue(settings.value("MaxSpeed", 100).toInt());
    ui->speedaccel->setValue(settings.value("Accel", 50).toInt());
    ui->speedKp->setValue(settings.value("Kp", 1).toInt());
    ui->speedKi->setValue(settings.value("Ki", 0).toInt());
    ui->speedKd->setValue(settings.value("Kd", 0).toInt());
    settings.endGroup();

    timer1sec = new QTimer(this);
    connect(timer1sec, &QTimer::timeout, this, &SpeedMode::onesec);
    timer1sec->start(1000);

    ui->speedgo->setChecked(false);
    go(false);
    connect(joystick, &Joystick::update, this, &SpeedMode::joystickUpdate);
    sensors->autoPing = true;   //start autopinger
}

SpeedMode::~SpeedMode()
{
    QSettings settings;
    settings.beginGroup("SpeedMode");
    settings.setValue("MaxSpeed", ui->speedmax->value());
    settings.setValue("Accel", ui->speedaccel->value());
    settings.setValue("Kp", ui->speedKp->value());
    settings.setValue("Ki", ui->speedKi->value());
    settings.setValue("Kd", ui->speedKd->value());
    settings.endGroup();

    sensors->autoPing = false;   // stop autopinger
}

void SpeedMode::joystickUpdate()
{
    ui->speedgo->setChecked(joystick->dmh);
}

void SpeedMode::onesec()
{
    ui->speedloops->setValue(loopcount);
    loopcount = 0;
}

void SpeedMode::go(bool checked)
{
    if (checked) {
        // Now running, so action of button would be to stop
        qDebug() << "Start Run";
        ui->speedgo->setText("Stop");
        speed.setAccel(ui->speedaccel->value());
        speed.setDirection(0);
        speed.setSpeed(-ui->speedmax->value());
        pid.setKp(ui->speedKp->value());
        pid.setKi(ui->speedKi->value());
        pid.setKd(ui->speedKd->value());
        lastdistance = sensors->totalDistance;
        pid.reset();
        timer.start();
    } else {
        // Now stopped, so action of button would be to start
        qDebug() << "Stop Run";
        ui->speedgo->setText(" <<<<<<<<<<  Go  <<<<<<<<<<");
        speed.stop();
    }
}

int SpeedMode::calcDirection() {
    int direrror = sensors->pingLeft - sensors->pingRight;
    loopcount++;
    int retval = pid.update(direrror) / 100;
    qDebug() << sensors->pingLeft << sensors->pingRight << direrror << retval;
    if (sensors->pingLeft + sensors->pingRight > 400) {
        // off the track
        return 0;
    }
    return retval;
}

void SpeedMode::idle() {
    ui->speedleftping->setValue(sensors->pingLeft);
    ui->speedrightping->setValue(sensors->pingRight);

    if (ui->speedgo->isChecked()) {
        // We're running
        ui->speedtime->setValue(timer.elapsed());
        speed.setDirection(calcDirection());
    }
    ui->speedspeed->setValue(-speed.update());
    ui->speeddir->setValue(speed.direction());
}
