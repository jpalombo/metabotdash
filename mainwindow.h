#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "camera.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    CCamera* cam;
    int fps;
    QTimer *timer0;
    QTimer *timer1sec;
    QTimer *timer1min;

    void setLightsOn(bool);

private slots:
    void on_pushButton_exit_clicked();
    void capture();
    void onesec();
    void unblank();
    void on_manual_toggled(bool checked);
    void on_speed_toggled(bool checked);
    void on_maze_toggled(bool checked);
    void on_line_toggled(bool checked);
    void on_test_toggled(bool checked);
};

#endif // MAINWINDOW_H
