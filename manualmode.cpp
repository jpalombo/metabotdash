#include <QSettings>
#include "manualmode.h"

ManualMode::ManualMode(Ui::MainWindow* mui, Joystick* mjoystick, Sensors* msensors, CCamera *mcam) :
    AbstractMode(mui, mjoystick, msensors, mcam)
{
    ui->pages->setCurrentWidget(ui->manualpage);
    connect(joystick, &Joystick::update, this, &ManualMode::joystickUpdate);
    speed.setAccel(0);
    speed.stop();

    QSettings settings;
    settings.beginGroup("ManualMode");
    ui->servo1min->setValue(settings.value("Servo1Min", 0).toInt());
    ui->servo1max->setValue(settings.value("Servo1Max", 255).toInt());
    ui->servo2min->setValue(settings.value("Servo2Min", 0).toInt());
    ui->servo2max->setValue(settings.value("Servo2Max", 255).toInt());
    ui->launchermaxfwd->setValue(settings.value("LauncherMaxFwd", 65535).toInt());
    ui->launchermaxrev->setValue(settings.value("LauncherMaxRev", 0).toInt());
    ui->launcherstop->setValue(settings.value("LauncherStop", 32767).toInt());
    ui->manualmax1->setValue(settings.value("ManualMax1", 20).toInt());
    ui->manualmax2->setValue(settings.value("ManualMax2", 100).toInt());
    ui->manualsteer->setValue(settings.value("Steer", 50).toInt());
    settings.endGroup();
}

ManualMode::~ManualMode() {
    QSettings settings;
    settings.beginGroup("ManualMode");
    settings.setValue("Servo1Min", ui->servo1min->value());
    settings.setValue("Servo1Max", ui->servo1max->value());
    settings.setValue("Servo2Min", ui->servo2min->value());
    settings.setValue("Servo2Max", ui->servo2max->value());
    settings.setValue("LauncherMaxFwd", ui->launchermaxfwd->value());
    settings.setValue("LauncherMaxRev", ui->launchermaxrev->value());
    settings.setValue("LauncherStop", ui->launcherstop->value());
    settings.setValue("ManualMax1", ui->manualmax1->value());
    settings.setValue("ManualMax2", ui->manualmax2->value());
    settings.setValue("Steer", ui->manualsteer->value());
    settings.endGroup();
}

int sign(int x) {
    if (x < 0)
        return -1;
    return 1;
}

int exp(int x) {
    return (x * x * sign(x)) >> 23;
}

int adjustrange (int x, int r1low, int r1high, int r2low, int r2high)
{
    return r2low + ((r2high - r2low) * (x - r1low) / (r1high - r1low));
}

void ManualMode::joystickUpdate(int eType, int eNumber, int eValue)
{
    Q_UNUSED(eType);
    Q_UNUSED(eNumber);
    Q_UNUSED(eValue);

    ui->posx->setText(QString::number(joystick->posx));
    ui->posy->setText(QString::number(joystick->posy));
    ui->dmh->setText(QString::number(joystick->dmh));

    int maxspeed;
    if (ui->first->isChecked())
        maxspeed = ui->manualmax1->value();
    else
        maxspeed = ui->manualmax2->value();

    int newspeed = adjustrange(-exp(joystick->posy), 0, 128, 0, maxspeed);
    int newdir = adjustrange(-exp(joystick->posx), 0, 128, 0, maxspeed * ui->manualsteer->value() / 100);

    ui->posxx->setText(QString::number(newspeed));
    ui->posyx->setText(QString::number(newdir));

    speed.setSpeedDir(newspeed, newdir);
}

void ManualMode::idle()
{
    ui->servo0->setText(QString::number(joystick->axis[1]));
    ui->servo1->setText(QString::number(joystick->axis[3]));
    ui->servo2->setText(QString::number(joystick->axis[4]));

    if (joystick->axis[1] > 0)
        sensors->servo[0] = adjustrange(joystick->axis[1], 0, 32767, ui->launcherstop->value(), ui->launchermaxfwd->value());
    else
        sensors->servo[0] = adjustrange(joystick->axis[1], -32767, 0, ui->launchermaxrev->value(), ui->launcherstop->value());
    sensors->servo[1] = adjustrange(joystick->axis[3], -32767, 32767, ui->servo1min->value(), ui->servo1max->value());
    sensors->servo[2] = adjustrange(joystick->axis[4], -32767, 32767, ui->servo2min->value(), ui->servo2max->value());

    ui->servo0x->setText(QString::number(sensors->servo[0]));
    ui->servo1x->setText(QString::number(sensors->servo[1]));
    ui->servo2x->setText(QString::number(sensors->servo[2]));

    static bool debounce = true;
    if (joystick->axis[6] > 0 && debounce)
        ui->manualsteer->stepUp();
    else if (joystick->axis[6] < 0 && debounce)
        ui->manualsteer->stepDown();
    if (joystick->axis[6] == 0)
        debounce = true;
    else
        debounce = false;


}
