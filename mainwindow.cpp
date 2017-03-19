#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Set-up Application
    QCoreApplication::setOrganizationName("Metabot");
    QCoreApplication::setOrganizationDomain("metaswitch.com");
    QCoreApplication::setApplicationName("Metabot3 Dashboard");

    // Set-up variables
    modes = new Modes(ui, this);

    // Adjust screen
    int h = ui->glWidget->height();
    ui->glWidget->setMaximumWidth(h * 3 / 4);
    ui->manual->setChecked(true);
    on_manual_toggled(true);
    unblank();      // make sure screen is not blanked
    ui->buildlabel->setText("Build:" + QString(__TIME__) + ", " + QString(__DATE__));

    // start unblank timer
    timer1min = new QTimer(this);
    connect(timer1min, SIGNAL(timeout()), this, SLOT(unblank()));
    timer1min->start(60000);
}

MainWindow::~MainWindow()
{
    delete modes;
    delete ui;
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
        modes->setMode(Modes::Manual);
}

void MainWindow::on_speed_toggled(bool checked)
{
    if (checked)
        modes->setMode(Modes::Speed);
}

void MainWindow::on_maze_toggled(bool checked)
{
    if (checked)
        modes->setMode(Modes::Maze);
}

void MainWindow::on_line_toggled(bool checked)
{
    if (checked)
        modes->setMode(Modes::Line);
}

void MainWindow::on_test_toggled(bool checked)
{
    if (checked)
        modes->setMode(Modes::Status);
}

void MainWindow::on_speedgo_toggled(bool checked)
{
    modes->buttonClicked(AbstractMode::Buttons::Go, checked);
}

void MainWindow::on_mazego_toggled(bool checked)
{
    modes->buttonClicked(AbstractMode::Buttons::Go, checked);
}

void MainWindow::on_linego_toggled(bool checked)
{
    modes->buttonClicked(AbstractMode::Buttons::Go, checked);
}

void MainWindow::on_configCompass_clicked(bool checked)
{
    modes->buttonClicked(AbstractMode::ConfigCompass, checked);
}

void MainWindow::on_configGyro_clicked(bool checked)
{
    modes->buttonClicked(AbstractMode::ConfigGyro, checked);
}

void MainWindow::on_mazeResetBearing_clicked(bool checked)
{
    modes->buttonClicked(AbstractMode::ConfigGyro, checked);
}

