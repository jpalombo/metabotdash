#include <QTextStream>
#include <QSettings>

#include "mazemode.h"

MazeMode::MazeMode(Ui::MainWindow* mui, Joystick* mjoystick, Sensors* msensors, CCamera *mcam) :
    AbstractMode(mui, mjoystick, msensors, mcam), pingLeft(10), pingRight(10), pingFront(10)
{
    ui->pages->setCurrentWidget(ui->mazepage);

    QSettings settings;
    settings.beginGroup("MazeMode");
    ui->mazemax->setValue(settings.value("MaxSpeed", 10).toInt());
    ui->mazeaccel->setValue(settings.value("Accel", 20).toInt());
    ui->mazeKping->setValue(settings.value("Kping", 10).toInt());
    ui->mazeKbearing->setValue(settings.value("Kbearing", 7).toInt());
    ui->mazecornererr->setValue(settings.value("CornerError", 150).toInt());
    settings.endGroup();

    connect(joystick, &Joystick::update, this, &MazeMode::joystickUpdate);
    sensors->autoPing = 2;   //start autopinger on left, right and front

    file.setFileName("/home/pi/metabot3/mazetest.csv");
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);
    out << "distance(* 0.5mm), pingLeft, pingRight, pingFront, sm pingLeft, sm pingRight, sm pingFront, speed, direction, bearing\n";

    speed.stop();
}

MazeMode::~MazeMode()
{
    go(false);
    file.close();
    QSettings settings;
    settings.beginGroup("MazeMode");
    settings.setValue("MaxSpeed", ui->mazemax->value());
    settings.setValue("Accel", ui->mazeaccel->value());
    settings.setValue("Kping", ui->mazeKping->value());
    settings.setValue("Kbearing", ui->mazeKbearing->value());
    settings.setValue("CornerError", ui->mazecornererr->value());
    settings.endGroup();

    sensors->autoPing = 0;   // stop autopinger
    speed.stop();
}

void MazeMode::go(bool checked)
{
    if (checked) {
        // Now running, so action of button would be to stop
        QTextStream out(&file);
        out << "Start\n";
        ui->mazego->setText("Stop");
        speed.setAccel(ui->mazeaccel->value());
        sensors->resetBearing();
        pingLeft.reset();
        pingRight.reset();
        pingFront.reset();
        state = 0;
    } else {
        // Now stopped, so action of button would be to start
        QTextStream out(&file);
        out << "Stop\n";
        ui->mazego->setText(" <<<<<<<<<<  Go  <<<<<<<<<<");
        speed.stop();
    }
}

void MazeMode::joystickUpdate(int, int, int)
{
    ui->mazego->setChecked(joystick->dmh);
    speed.setSpeedDir(-joystick->posy/2000, -joystick->posx/2000);
}

void MazeMode::buttonClicked(AbstractMode::Buttons button, bool checked)
{
    if (button == ConfigGyro && checked) {
        sensors->configGyro(2000);
        ui->mazeResetBearing->setChecked(false);
    }
}

void MazeMode::idle()
{
    int distance = sensors->totalDistance;
    pingLeft.update((int)sensors->pingLeft, distance);
    pingRight.update((int)sensors->pingRight, distance);
    pingFront.update((int)sensors->pingFront, distance);
    ui->mazeleftping->setValue((int)sensors->pingLeft);
    ui->mazerightping->setValue((int)sensors->pingRight);
    ui->mazefrontping->setValue((int)sensors->pingFront);
    ui->mazespeed->setValue(speed.speed());
    ui->mazedir->setValue(speed.direction());
    ui->mazebearing->setValue(sensors->bearing/10);

    if (ui->mazego->isChecked()) {

        calcDirection();
        QTextStream out(&file);
        out << distance << ", "
            << sensors->pingLeft  << ", "
            << sensors->pingRight << ", "
            << sensors->pingFront << ", "
            << pingLeft.value()   << ", "
            << pingRight.value()  << ", "
            << pingFront.value()  << ", "
            << speed.speed()      << ", "
            << speed.direction()  << ", "
            << sensors->bearing   << "\n";
    }
}

void MazeMode::followWall(Wall wall, int clearance, int bearing)
{
    targetWall = wall;
    targetWallClearance = clearance;
    targetBearing = bearing;
}

void MazeMode::turnright()
{
    speed.setSpeedDir(ui->mazemax->value()/2, -ui->mazemax->value());
    followWall(NoWall, 0, 0);
}

void MazeMode::turnleft()
{
    speed.setSpeedDir(ui->mazemax->value()/2, ui->mazemax->value());
    followWall(NoWall, 0, 0);
}

void MazeMode::goforward()
{
    speed.setSpeedDir(ui->mazemax->value(), 0);
}

void MazeMode::calcDirection()
{
    static int startdist;
    /* Follow a series of targets
     * 0. Starting position, stopped
     * 1. Forward following left wall until right wall ping jumps up to ~650mm
     * 2. Turn right until bearing = 90
     * 3. Forward following left wall until right wall ping jumps to ~300mm
     * 4. Turn right until bearing = 180
     * 5. Forward until front ping < 200
     * 6. Turn right until bearing = 270
     * 7. Forward until front ping < 300 Follow left wall
     * 71. Forward until front ping < 300 Follow right wall
     * 8. Turn left until bearing = 180
     * 9. Forward until front ping < 200
     * 10. Turn left until bearing = 90
     * 11. Forward following right wall until left wall ping jumps to ~650mm
     * 12. Turn left until bearing = 0
     * 13. Forward following right wall until left or right wall ping jumps to ~650mm
     * 14. Stop
    */

    int cerr = ui->mazecornererr->value();

    QTextStream out(&file);

    switch (state) {
    case 0: // Stopped at start
        speed.setSpeed(ui->mazemax->value());
        followWall(LeftWall, 70, 0);
        state = 1;
        out << "Move to state 1\n";
        break;
    case 1: // First straight
        if (pingFront.value() < 360  && pingFront.unconfidence() < 10) {
            turnright();
            state = 2;
            out << "Move to state 2\n";
        }
        break;
    case 2: // First right turn
        if (sensors->bearing >= (900 - cerr)) {
            speed.stop();
            state = 3;
            out << "Move to state 3\n";
        }
        break;
    case 3: // Second Straight
        if (speed.speed() == 0) {
            goforward();
            followWall(LeftWall, 70, 900);
        }
        if (pingFront.value() < 800 && pingRight.value() > 300) {
            state = 4;
            turnright();
            out << "Move to state 4\n";
        }
        break;
    case 4:
        // turn right in hairpin
        if (sensors->bearing >= (1800 - cerr)) {
            speed.stop();
            state = 5;
            out << "Move to state 5\n";
        }
        break;
    case 5: // First Short Straight in Hairpin
        if (speed.speed() == 0) {
            goforward();
        }
        if (pingFront.value() < 350 && pingFront.unconfidence() < 10) {
            state = 6;
            turnright();
            out << "Move to state 6\n";
        }
        break;
    case 6:
        // second right in hairpin
        if (sensors->bearing >= (2700 - cerr)) {
            speed.stop();
            state = 7;
            out << "Move to state 7\n";
        }
        break;
    case 7: // Forward through hairpin
        if (speed.speed() == 0) {
            goforward();
            followWall(LeftWall, 70, 2700);
        }
        if (pingRight.value() < 300) {
            state = 72;
            followWall(RightWall, 70, 2700);
            out << "Move to state 72\n";
        }
        if (pingLeft.value() > 300) {
            state = 71;
            followWall(NoWall, 0, 2700);
            out << "Move to state 71\n";
        }
        break;
    case 71: // Forward through hairpin cant see right wall or left wall
        if (pingRight.value() > 300) {
            followWall(RightWall, 70, 2700);
            state = 72;
            out << "Move to state 72\n";
        }
        break;
    case 72: // Forward through hairpin, can see right wall
        if (pingLeft.value() > 300) {
            state = 8;
            startdist = sensors->totalDistance;
            out << "Move to state 8\n";
        }
        break;
    case 8: // Move forward a little bit (~15cm)
        if ((sensors->totalDistance - startdist) > 1500) {
            state = 81;
            speed.stop();
            out << "Move to state 81\n";
        }
        break;
    case 81:
        // turn left in hairpin
        if (speed.speed() == 0) {
            turnleft();
        }
        if (sensors->bearing <= (1800 + cerr)) {
            speed.stop();
            state = 9;
            out << "Move to state 9\n";
        }
        break;
    case 9: // Second Short Straight in Hairpin
        if (speed.speed() == 0) {
            goforward();
        }
        if (pingFront.value() < 240 && pingFront.unconfidence() < 10) {
            state = 10;
            turnleft();
            out << "Move to state 10\n";
        }
        break;
    case 10:
        // second left in hairpin
        if (sensors->bearing <= (1000 + cerr)) {
            speed.stop();
            state = 11;
            out << "Move to state 11\n";
        }
        break;
    case 11: // Third Long Straight
        if (speed.speed() == 0) {
            goforward();
            followWall(RightWall, 70, 900);
        }
        if (pingFront.value() < 250 && pingFront.unconfidence() < 10 && pingLeft.value() > 300) {
            state = 12;
            turnleft();
            out << "Move to state 12\n";
        }
        break;
    case 12: // Last left turn
        if (sensors->bearing <= (cerr)) {
            speed.stop();
            state = 13;
            out << "Move to state 13\n";
        }
        break;
    case 13: // Final Long Straight
        if (speed.speed() == 0) {
            goforward();
            followWall(RightWall, 70, 0);
        }
        if (pingRight.value() >300 && pingLeft.value() > 300) {
            state = 14;
            followWall(NoWall, 0, 0);
            startdist = sensors->totalDistance;
            out << "Move to state 14\n";
        }
        break;
    case 14:
        if ((sensors->totalDistance - startdist) > 2000) {
            speed.stop();
        }
        break;
    }

    // Finally adjust direction to follow the appropriate wall
    int pingdir = 0;
    if (targetWall == LeftWall)
        pingdir = (pingLeft.value() - targetWallClearance) * ui->mazeKping->value();
    else if (targetWall == RightWall)
        pingdir = ( targetWallClearance - pingRight.value()) * ui->mazeKping->value();
    else
        return;

    int bearingdir = (sensors->bearing - targetBearing) * ui -> mazeKbearing->value();
    int totaldir = bearingdir + pingdir;
    if (totaldir > 250)
        totaldir = 250;
    if (totaldir < -250)
        totaldir = -250;

    speed.setDirection(totaldir * speed.speed()/ 500);
}

