#ifndef LINEMODE_H
#define LINEMODE_H

#include <QTimer>
#include <opencv2/core/core.hpp>
#include "abstractmode.h"

using namespace cv;

class LineMode : public AbstractMode
{
    Q_OBJECT
public:
    LineMode(Ui::MainWindow *mui, Joystick *mjoystick, Sensors *msensors, CCamera *mcam);
    ~LineMode();
    void idle();
    void joystickUpdate(int eType, int eNumber, int eValue);
    void go(bool checked);

private:
    int nextimage;
    Mat saveimage[16];
    QTimer *timer1sec;
    int fps;
    const int halfrange;
    const int topleft;
    const int topright;
    const int bottomleft;
    const int bottomright;


    void setLightsOn(bool lightsOn);
    void processFrame(Mat frame);
    int etoi(int e);
    int find(Mat frame, int colour, int start, int direction);

public slots:
    void capture();
    void onesec();
};

#endif // LINEMODE_H
