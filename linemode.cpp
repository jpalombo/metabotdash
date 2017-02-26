#include "linemode.h"
#include <wiringPi.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

LineMode::LineMode(Ui::MainWindow* mui, Joystick* mjoystick, Sensors* msensors, CCamera *mcam) :
    AbstractMode(mui, mjoystick, msensors, mcam), fps(0)
{
    ui->pages->setCurrentWidget(ui->linepage);
    setLightsOn(true);

    timer1sec = new QTimer(this);
    connect(timer1sec, SIGNAL(timeout()), this, SLOT(onesec()));
    timer1sec->start(1000);
}

LineMode::~LineMode()
{
    setLightsOn(false);
}

void LineMode::LineMode::idle()
{
    capture();
}

void LineMode::setLightsOn(bool lightsOn)
{
    if (lightsOn)
        digitalWrite (27, HIGH);
    else
        digitalWrite (27, LOW);
}

Mat gray;
Mat gray2;
Mat dispmat;
void LineMode::capture()
{
    // Capture image
    // lock the chosen frame buffer, and copy it directly into the corresponding open gl texture
    const void* frame_data; int frame_sz;

    if(cam->BeginReadFrame(0,frame_data,frame_sz))
    {
        //Mat image(480, 640, CV_8UC4, (void *)frame_data);
        //cvtColor(image, gray, COLOR_RGBA2GRAY);
        //ui->glWidget->SetPixels(dispmat.data);

        ui->glWidget->setPixels(frame_data);
        cam->EndReadFrame(0);
    }

    // Display image
    ui->glWidget->update();
    fps++;
}

void LineMode::onesec()
{
    ui->fps->setText("fps :" + QString::number(fps));
    fps = 0;
}
