#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QTimer>
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

    ui->buildlabel->setText("Build:" + QString(__TIME__) + ", " + QString(__DATE__));

    // init wiringPi
    wiringPiSetupSys();
    pinMode (17, OUTPUT);
    digitalWrite (17, LOW);

    //init camera
    cam = StartCamera(640, 480, 30, 1, true);

    QTimer *timer0 = new QTimer(this);
    connect(timer0, SIGNAL(timeout()), this, SLOT(capture()));
    timer0->start();

    QTimer *timer1 = new QTimer(this);
    connect(timer1, SIGNAL(timeout()), this, SLOT(onesec()));
    timer1->start(1000);
}

MainWindow::~MainWindow()
{
    StopCamera();
    delete ui;
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
        Mat image(480, 640, CV_8UC4, (void *)frame_data);
        cvtColor(image, gray, COLOR_RGBA2GRAY);
        Canny(gray, gray2, ui->lowerT->value(), ui->upperT->value());
        cvtColor(gray2, dispmat, COLOR_GRAY2RGBA);

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

void MainWindow::on_pushButton_exit_clicked()
{
    close();
}

void MainWindow::on_lowerT_sliderMoved(int position)
{
    ui->lowerV->setText(QString::number(position));
}

void MainWindow::on_upperT_sliderMoved(int position)
{
    ui->upperV->setText(QString::number(position));
}

void MainWindow::on_lights_clicked(bool checked)
{
    if (checked)
        digitalWrite (17, HIGH);
    else
        digitalWrite (17, LOW);
}
