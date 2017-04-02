#include <QSettings>
#include <QDebug>
#include "linemode.h"
#include <wiringPi.h>
#include <opencv2/imgproc/imgproc.hpp>
//#include <opencv2/highgui/highgui.hpp>
#include <lodepng/lodepng.h>

LineMode::LineMode(Ui::MainWindow* mui, Joystick* mjoystick, Sensors* msensors, CCamera *mcam) :
    AbstractMode(mui, mjoystick, msensors, mcam), nextimage(0), fps(0),
    halfrange(CAMWIDTH + CAMHEIGHT - 2),
    topleft(-CAMWIDTH/2),
    topright((CAMWIDTH/2)-1),
    bottomleft(topleft - CAMHEIGHT + 1),
    bottomright(topright + CAMHEIGHT - 1)

{
    ui->pages->setCurrentWidget(ui->linepage);
    setLightsOn(true);
    speed.stop();

    QSettings settings;
    settings.beginGroup("LineMode");
    ui->linethreshold->setValue(settings.value("Threshold", 200).toInt());
    ui->linespeed->setValue(settings.value("Speed", 10).toInt());
    ui->lineaccel->setValue(settings.value("Accel", 100).toInt());
    ui->linespeedfactor->setValue(settings.value("SpeedFactor", 10).toInt());
    ui->linedirfactor->setValue(settings.value("DirFactor", 10).toInt());
    ui->lineerroroffset->setValue(settings.value("ErrorOffset", 80).toInt());
    settings.endGroup();

    connect(joystick, &Joystick::update, this, &LineMode::joystickUpdate);

    timer1sec = new QTimer(this);
    connect(timer1sec, SIGNAL(timeout()), this, SLOT(onesec()));
    timer1sec->start(1000);
}

LineMode::~LineMode()
{
    setLightsOn(false);
    QSettings settings;
    settings.beginGroup("LineMode");
    settings.setValue("Threshold", ui->linethreshold->value());
    settings.setValue("Speed", ui->linespeed->value());
    settings.setValue("Accel", ui->lineaccel->value());
    settings.setValue("SpeedFactor", ui->linespeedfactor->value());
    settings.setValue("DirFactor", ui->linedirfactor->value());
    settings.setValue("ErrorOffset", ui->lineerroroffset->value());
    settings.endGroup();
}

void LineMode::joystickUpdate(int eType, int eNumber, int eValue)
{
    Q_UNUSED(eType);
    Q_UNUSED(eNumber);
    Q_UNUSED(eValue);

    ui->linego->setChecked(joystick->dmh);
    speed.setSpeedDir(-joystick->posy/2000, -joystick->posx/2000);

//    if (eType == 1 && eNumber == 0 && eValue == 1) {
//        // square button pressed
//        qDebug() << "Take snapshot...";
//        std::string fname;
//        for (int i = 0; i < 16; i++) {
//            int imageindex = (nextimage + 1 + i) % 16;
//            fname = std::string("/home/pi/metabot3/lineimage") + std::to_string(i) + std::string(".png");
//            lodepng::encode(fname, (const unsigned char*)saveimage[imageindex].data, CAMWIDTH, CAMHEIGHT, LCT_GREY);
//        }
//           }
}

void LineMode::LineMode::idle()
{
    capture();
    ui->linespeedactual->setText("Speed : " + QString::number(speed.speed()));
    ui->linediractual->setText("Dir : "+ QString::number(speed.direction()));
}

void LineMode::go(bool checked)
{
    if (checked) {
        // Now running, so action of button would be to stop
        ui->linego->setText("Stop");
        speed.setAccel(ui->lineaccel->value());
        speed.setDirection(0);
        speed.setSpeed(ui->linespeed->value());
    } else {
        // Now stopped, so action of button would be to start
        ui->linego->setText(" <<<<< Go <<<<<");
        speed.stop();
    }
}

void LineMode::setLightsOn(bool lightsOn)
{
    if (lightsOn)
        digitalWrite (27, HIGH);
    else
        digitalWrite (27, LOW);
}

void LineMode::capture()
{
    // Capture image
    // lock the chosen frame buffer, and copy it directly into the corresponding open gl texture
    const void* frame_data; int frame_sz;

    if(cam->BeginReadFrame(0,frame_data,frame_sz))
    {
        Mat image(CAMWIDTH, CAMHEIGHT, CV_8UC1, (void *)frame_data);
        threshold(image, image, ui->linethreshold->value(), 255, THRESH_BINARY);
        saveimage[nextimage] = image.clone();
        processFrame(image);
        nextimage = (nextimage+1) % 16;
        ui->glWidget->setPixels(image.data);
        cam->EndReadFrame(0);
    }

    // Display image
    ui->glWidget->update();
    fps++;
}

int LineMode::etoi(int e)
{
    // Edge to index
    //
    // takes a value that counts around the edge of the frame and converts to an index into the frame
    // Origin is in the middle of the top row, so assuming a width of 96 and height of 64
    // Top Left = -48
    // Top Right = 47
    // Bottom Left = -111
    // Bottom Right = 110
    // Range = -158 to 157

    const int maxscreen = (CAMWIDTH * CAMHEIGHT) - 1;

    // normalise e
    while (e >= halfrange)
        e -= halfrange*2;
    while (e < -halfrange)
        e += halfrange*2;

    // check if e on the top edge
    if (e >= topleft && e <= topright)
        return maxscreen - (e + CAMWIDTH/2);

    if (e > 0) {  // On the right hand half
        if (e <= bottomright)   // right hand edge
            return maxscreen - ((CAMWIDTH * (e + 1 - topright)) - 1);
        // else on the bottom edge
        return maxscreen - ((CAMWIDTH * CAMHEIGHT) - (e - bottomright + 1));
    } else { // On the left hand half
        if (e >= bottomleft)   // left hand edge
            return maxscreen - (CAMWIDTH * (topleft - e));
        // else on the bottom edge
        return maxscreen - ((CAMWIDTH * (CAMHEIGHT - 1)) + (bottomleft - e));
    }
}

#define WHITE 255
#define BLACK 0
#define GRAY 200

int LineMode::find(Mat frame, int colour, int start, int direction) {
    while (frame.data[etoi(start)] != colour && start > -halfrange && start < halfrange)
        start += direction;
    return start;
}

void LineMode::processFrame(Mat frame)
{
    static int starthint = 0;
    int startblack = 0;
    int endblack = 0;

    if (frame.data[etoi(starthint)] == WHITE) {  // Not on the line, need to find it
        int fwdblack = find(frame, BLACK, starthint, 1);
        int backblack = find(frame, BLACK, starthint, -1);
        if (qAbs(fwdblack) < qAbs(backblack))
            starthint = fwdblack;
        else
            starthint = backblack;
    }

    if (frame.data[etoi(starthint)] == BLACK) {  // On the line, look for white either way
        startblack = find(frame, WHITE, starthint, -1);
        endblack = find(frame, WHITE, starthint, 1);
    }

    int midblack;
    if (startblack < topleft && endblack > topleft)
        midblack = topleft;
    else if (startblack < topright && endblack > topright)
        midblack = topright;
    else
        midblack = (startblack + endblack) / 2;

    if (frame.data[etoi(midblack)] == BLACK) {
        frame.data[etoi(midblack)] = GRAY;
        starthint = midblack;

        if (ui->linego->isChecked()){
            int error = ui->lineerroroffset->value() - qAbs(midblack);
            int newspeed = error * ui->linespeedfactor->value() * ui->linespeed->value() / 500;
            int newdir = -midblack * ui->linedirfactor->value() * ui->linespeed->value() / 500;
            speed.setSpeedDir(newspeed, newdir);
        }
    }
    else {
        starthint = 0;
    }
}

void LineMode::onesec()
{
    ui->fps->setText("fps :" + QString::number(fps));
    fps = 0;
}
