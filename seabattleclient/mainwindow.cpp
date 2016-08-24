#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QPalette pal;
    pal.setBrush(ui->pictureMain->backgroundRole(), QBrush(QPixmap(":/start.png")));
    ui->pictureMain->setPalette(pal);
}

MainWindow::~MainWindow()
{
    delete ui;
}
