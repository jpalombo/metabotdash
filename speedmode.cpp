#include <QSettings>
#include <QTextStream>
#include "speedmode.h"

SpeedMode::SpeedMode(Ui::MainWindow* mui, Joystick* mjoystick, Sensors* msensors, CCamera *mcam) :
    AbstractMode(mui, mjoystick, msensors, mcam), pid(1,0,0), loopcount(0)
{
    ui->pages->setCurrentWidget(ui->speedpage);

    QSettings settings;
    settings.beginGroup("SpeedMode");
    ui->speedmax->setValue(settings.value("MaxSpeed", 100).toInt());
    ui->speedaccel->setValue(settings.value("Accel", 50).toInt());
    ui->speedbias->setValue(settings.value("Bias", 0).toInt());
    ui->speedKp->setValue(settings.value("Kp", 1).toInt());
    ui->speedKi->setValue(settings.value("Ki", 0).toInt());
    ui->speedKd->setValue(settings.value("Kd", 0).toInt());
    settings.endGroup();

    timer1sec = new QTimer(this);
    connect(timer1sec, &QTimer::timeout, this, &SpeedMode::onesec);
    timer1sec->start(1000);

    file.setFileName("/home/pi/metabot3/speedtest.csv");
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);
    out << "time(ms), distance(* 0.5mm), pingLeft, pingRight, direrror, speed, direction\n";

    ui->speedgo->setChecked(false);
    go(false);
    connect(joystick, &Joystick::update, this, &SpeedMode::joystickUpdate);
    sensors->autoPing = 1;   //start autopinger on left and right only
}

SpeedMode::~SpeedMode()
{
    go(false);
    file.close();
    QSettings settings;
    settings.beginGroup("SpeedMode");
    settings.setValue("MaxSpeed", ui->speedmax->value());
    settings.setValue("Accel", ui->speedaccel->value());
    settings.setValue("Bias", ui->speedbias->value());
    settings.setValue("Kp", ui->speedKp->value());
    settings.setValue("Ki", ui->speedKi->value());
    settings.setValue("Kd", ui->speedKd->value());
    settings.endGroup();

    sensors->autoPing = 0;   // stop autopinger
}

void SpeedMode::joystickUpdate(int, int, int)
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
        QTextStream out(&file);
        out << "Start Run \n";
        ui->speedgo->setText("Stop");
        speed.setAccel(ui->speedaccel->value());
        speed.setDirection(ui->speedbias->value());
        speed.setSpeed(ui->speedmax->value());
        pid.setKp(ui->speedKp->value());
        pid.setKi(ui->speedKi->value());
        pid.setKd(ui->speedKd->value());
        lastdistance = sensors->totalDistance;
        direrror = sensors->pingLeft - sensors->pingRight;
        pid.reset(direrror);
        int pingwidth = sensors->pingLeft + sensors->pingRight;
        pinglower = pingwidth * 8 / 10;
        pinghigher = pingwidth * 12 / 10;
        timer.start();
    } else {
        // Now stopped, so action of button would be to start
        ui->speedgo->setText(" <<<<<<<<<<  Go  <<<<<<<<<<");
        speed.setAccel(500);
        speed.stop();
    }
}

void SpeedMode::idle() {
    ui->speedleftping->setValue(sensors->pingLeft);
    ui->speedrightping->setValue(sensors->pingRight);
    ui->speedspeed->setValue(speed.speed());
    ui->speeddir->setValue(speed.direction());

    if (ui->speedgo->isChecked()) {
        // We're running
        ui->speedtime->setValue(timer.elapsed());
        speed.setDirection(calcDirection() + ui->speedbias->value());
        QTextStream out(&file);
        out << timer.elapsed() << ", " << sensors->totalDistance - lastdistance
            << ", " << sensors->pingLeft << ", " << sensors->pingRight << ", " << direrror << ", " << speed.speed()
            << ", " << speed.direction() << "\n";
    }
}

int SpeedMode::calcDirection() {
    loopcount++;
    int pingwidth = sensors->pingLeft + sensors->pingRight;
    if ((pingwidth > pinglower) && (pingwidth < pinghigher))
        direrror = ((direrror * 4) + sensors->pingLeft - sensors->pingRight) / 5;
    else
        return 0;

    int retval = pid.update(direrror) / 50;
    return retval;
}
