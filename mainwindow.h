#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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

private slots:
    void on_pushButton_exit_clicked();
    void capture();
    void onesec();
    void on_lowerT_sliderMoved(int position);
    void on_upperT_sliderMoved(int position);
    void on_lights_clicked(bool checked);
};

#endif // MAINWINDOW_H
