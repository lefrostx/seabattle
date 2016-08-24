#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    timer = new QTimer{this};
    connect(timer, &QTimer::timeout, this, &MainWindow::updateStatus);

    prepare();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateStatus()
{
    switch(status) {
        case Status::init: doInit(); break;
        case Status::wait: doWait(); break;
        case Status::ship: doShip(); break;
        case Status::load: doLoad(); break;
        case Status::play: doPlay(); break;
        case Status::stop: doStop(); break;
    }
}

void MainWindow::prepare()
{
    status = Status::init;
    timer->start(500);
}

void MainWindow::doInit()
{
    QPalette pal;
    pal.setBrush(ui->pictureMain->backgroundRole(), QBrush(QPixmap(":/start.png")));
    ui->pictureMain->setPalette(pal);

    status = Status::wait;
}

void MainWindow::doWait()
{

}

void MainWindow::doShip()
{

}

void MainWindow::doLoad()
{

}

void MainWindow::doPlay()
{

}

void MainWindow::doStop()
{

}
