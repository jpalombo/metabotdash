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
};

#endif // MAINWINDOW_H
