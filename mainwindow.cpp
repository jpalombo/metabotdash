#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QTimer>
#include <QFile>
#include <QProcess>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <wiringPi.h>

using namespace cv;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    fps(0)
{
    ui->setupUi(this);

    // unblank the screen
    unblank();

    ui->buildlabel->setText("Build:" + QString(__TIME__) + ", " + QString(__DATE__));

    wiringPiSetupGpio();
    pinMode(27, OUTPUT);
    setLightsOn(false);

    //init camera
    cam = StartCamera(1296, 972, 30, 1, true);
    //cam = StartCamera(640, 480, 30, 1, true);

    timer0 = new QTimer(this);
    connect(timer0, SIGNAL(timeout()), this, SLOT(capture()));
    // Don't start timer0 until line mode selected

    timer1sec = new QTimer(this);
    connect(timer1sec, SIGNAL(timeout()), this, SLOT(onesec()));
    timer1sec->start(1000);

    timer1min = new QTimer(this);
    connect(timer1min, SIGNAL(timeout()), this, SLOT(unblank()));
    timer1min->start(60000);
}

MainWindow::~MainWindow()
{
    setLightsOn(false);
    StopCamera();
    delete ui;
}

void MainWindow::setLightsOn(bool lightsOn)
{
    if (lightsOn)
        digitalWrite (27, HIGH);
    else
        digitalWrite (27, LOW);
}

Mat gray;
Mat gray2;
Mat dispmat;
void MainWindow::capture()
{
    // Capture image
    // lock the chosen frame buffer, and copy it directly into the corresponding open gl texture
    const void* frame_data; int frame_sz;

    if(cam->BeginReadFrame(0,frame_data,frame_sz))
    {
        //Mat image(480, 640, CV_8UC4, (void *)frame_data);
        //cvtColor(image, gray, COLOR_RGBA2GRAY);
        //ui->glWidget->SetPixels(dispmat.data);

        ui->glWidget->SetPixels(frame_data);
        cam->EndReadFrame(0);
    }

    // Display image
    ui->glWidget->update();
    fps++;
}

void MainWindow::onesec()
{
    ui->fps->setText("fps :" + QString::number(fps));
    fps = 0;
}

void MainWindow::unblank()
{
    QFile screen("/dev/tty1");
    if (!screen.open(QIODevice::WriteOnly)){
        return;
    }
    screen.write("\033[13]");
    screen.close();
}

void MainWindow::on_pushButton_exit_clicked()
{
    close();
}

void MainWindow::on_manual_toggled(bool checked)
{
    if (checked)
        ui->pages->setCurrentWidget(ui->manualpage);
}

void MainWindow::on_speed_toggled(bool checked)
{
    if (checked)
        ui->pages->setCurrentWidget(ui->speedpage);
}

void MainWindow::on_maze_toggled(bool checked)
{
    if (checked)
        ui->pages->setCurrentWidget(ui->mazepage);
}

void MainWindow::on_line_toggled(bool checked)
{
    setLightsOn(checked);
    if (checked) {
        timer0->start();
        ui->pages->setCurrentWidget(ui->linepage);
    }
    else {
        timer0->stop();
    }
}

void MainWindow::on_test_toggled(bool checked)
{
    if (checked)
        ui->pages->setCurrentWidget(ui->testpage);
}
