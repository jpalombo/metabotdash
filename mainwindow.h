#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "modes.h"

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
    QTimer *timer1min;
    Modes *modes;

private slots:
    void unblank();
    void on_pushButton_exit_clicked();
    void on_manual_toggled(bool checked);
    void on_speed_toggled(bool checked);
    void on_maze_toggled(bool checked);
    void on_line_toggled(bool checked);
    void on_test_toggled(bool checked);
    void on_speedgo_toggled(bool checked);
    void on_mazego_toggled(bool checked);
    void on_linego_toggled(bool checked);
    void on_configCompass_clicked(bool checked);
    void on_configGyro_clicked(bool checked);
    void on_mazeResetBearing_clicked(bool checked);
    void on_reboot_clicked();
    void on_speedResetBearing_clicked(bool checked);
    void on_halt_clicked();
    void on_April1_toggled(bool checked);
    void on_anotherjoke_clicked();
};

#endif // MAINWINDOW_H
