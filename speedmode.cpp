#include <QSettings>
#include <QTextStream>
#include "speedmode.h"

SpeedMode::SpeedMode(Ui::MainWindow* mui, Joystick* mjoystick, Sensors* msensors, CCamera *mcam) :
    AbstractMode(mui, mjoystick, msensors, mcam),
    pingLeft(10), pingRight(10),
    loopcount(0),
    bearingPid(1,0,0)
{
    ui->pages->setCurrentWidget(ui->speedpage);

    QSettings settings;
    settings.beginGroup("SpeedMode");
    ui->speedmax->setValue(settings.value("MaxSpeed", 100).toInt());
    ui->speedaccel->setValue(settings.value("Accel", 50).toInt());
    ui->speedKping->setValue(settings.value("Kping", 10).toInt());
    ui->speedKbearing->setValue(settings.value("Kbearing", 7).toInt());
    ui->speedKdBearing->setValue(settings.value("KdBearing", 7).toInt());
    settings.endGroup();

    timer1sec = new QTimer(this);
    connect(timer1sec, &QTimer::timeout, this, &SpeedMode::onesec);
    timer1sec->start(1000);

    file.setFileName("/home/pi/metabot3/speedtest.csv");
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);
    out << "time(ms), distance(* 0.5mm), pingLeft, pingRight, pingLeft Filt, pigRight Filt, Bearing , speed, direction\n";

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
    settings.setValue("Kping", ui->speedKping->value());
    settings.setValue("Kbearing", ui->speedKbearing->value());
    settings.setValue("KdBearing", ui->speedKdBearing->value());
    settings.endGroup();

    sensors->autoPing = 0;   // stop autopinger
}

void SpeedMode::joystickUpdate(int, int, int)
{
    ui->speedgo->setChecked(joystick->dmh);
}

void SpeedMode::buttonClicked(AbstractMode::Buttons button, bool checked)
{
    if (button == ConfigGyro && checked) {
        sensors->configGyro(2000);
        ui->speedResetBearing->setChecked(false);
    }
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
        out << "Start Run"
            << ", Kping =" << ui->speedKping->value()
            << ", Kbearing =" << ui->speedKbearing->value()
            << ", KdBearing =" << ui->speedKdBearing->value()
            << ", Max Speed =" << ui->speedmax->value()
            << ", Accel =" << ui->speedaccel->value() << "\n";
        ui->speedgo->setText("Stop");
        speed.setAccel(ui->speedaccel->value());
        speed.setDirection(0);
        speed.setSpeed(ui->speedmax->value());
        pingLeft.reset();
        pingRight.reset();
        bearingPid.setKp(ui->speedKbearing->value());
        bearingPid.setKd(ui->speedKdBearing->value());
        bearingPid.reset(0);

        startdistance = sensors->totalDistance;
        timer.start();
    } else {
        // Now stopped, so action of button would be to start
        ui->speedgo->setText(" <<<<<<<<<<  Go  <<<<<<<<<<");
        speed.setAccel(500);
        speed.stop();
    }
}

void SpeedMode::idle() {
    pingLeft.update(sensors->pingLeft, sensors->totalDistance);
    pingRight.update(sensors->pingRight, sensors->totalDistance);
    ui->speedleftping->setValue(sensors->pingLeft);
    ui->speedrightping->setValue(sensors->pingRight);
    ui->speedspeed->setValue(speed.speed());
    ui->speeddir->setValue(speed.direction());
    ui->speedbearing->setValue(sensors->bearing/10);

    if (ui->speedgo->isChecked()) {
        // We're running
        ui->speedtime->setValue(timer.elapsed());
        speed.setDirection(calcDirection());
        QTextStream out(&file);
        out << timer.elapsed() << ", "
            << sensors->totalDistance - startdistance << ", "
            << sensors->pingLeft << ", "
            << sensors->pingRight << ", "
            << pingLeft.value() <<  ", "
            << pingRight.value() << ", "
            << sensors->bearing <<  ", "
            << speed.speed() << ", "
            << speed.direction() << "\n";
    }
}

int SpeedMode::calcDirection() {
    loopcount++;
    int direrror = (pingLeft.value() - pingRight.value()) * ui->speedKping->value();
    int bearingerror = bearingPid.update(sensors->bearing) * ui->speedKbearing->value();
    return (bearingerror + direrror) * speed.speed()/ 500;
}
