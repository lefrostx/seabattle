#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPixmap>

namespace {
    const QString serverUrl{"http://localhost/seabattle/battle.php"};
    using SeaBattleClient::ServerInfo;
    using SeaBattleClient::ServerShip;
    using SeaBattleClient::ServerFire;
    using SeaBattleClient::ServerGame;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    timer{new QTimer{this}},
    serverInfo{new ServerInfo{serverUrl, this}},
    serverShip{new ServerShip{serverUrl, this}},
    serverFire{new ServerFire{serverUrl, this}},
    serverGame{new ServerGame{serverUrl, this}}
{
    ui->setupUi(this);

    connect(timer,          &QTimer::timeout,
            this,           &MainWindow::updateStatus);

    connect(serverInfo,     &ServerInfo::infoReceived,
            this,           &MainWindow::receiveInfo);

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

void MainWindow::receiveInfo()
{
    if (serverInfo->getStatus() == "wait") {
        if (serverInfo->getFreeOcean() >= 0) {
            myOcean = serverInfo->getFreeOcean();
            ui->pictureMain->clear();
            showMessage("Получен океан № " + QString::number(myOcean));
            status = Status::ship;
        } else {
            showMessage("Нет свободных океанов");
        }
    } else {
        showMessage("Игра уже идет, дождитесь окончания");
    }
}

void MainWindow::prepare()
{
    status = Status::init;
    timer->start(500);
}

void MainWindow::doInit()
{
    showMessage("Подготовка экрана");
    ui->pictureMain->setPixmap(QPixmap(":/start.png"));
    status = Status::wait;
}

void MainWindow::doWait()
{
    showMessage("Ожидание сервера");
    serverInfo->request();
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

void MainWindow::showMessage(const QString &message)
{
    ui->labelMessage->setText(message);
}
