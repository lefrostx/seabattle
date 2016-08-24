#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPixmap>

namespace {
    const QString serverUrl{"http://localhost/seabattle/battle.php"};
}

using SeaBattleClient::ServerInfo;
using SeaBattleClient::ServerShip;
using SeaBattleClient::ServerFire;
using SeaBattleClient::ServerGame;

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

    connect(serverShip,     &ServerShip::shipReceived,
            this,           &MainWindow::receiveShip);

    SeaBattleClient::Box::pictureMain = ui->pictureMain;
    SeaBattleClient::Box::boxClicked = [this](int ocean, int row, int col){return funcBoxClicked(ocean, row, col);};

    prepare();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::funcBoxClicked(int /*place*/, int /*row*/, int /*col*/)
{

}

void MainWindow::updateStatus()
{
    switch(status) {
        case Status::init:  doInit();   break;
        case Status::wait:  doWait();   break;
        case Status::ship:  doShip();   break;
        case Status::load:  doLoad();   break;
        case Status::play:  doPlay();   break;
        case Status::stop:  doStop();   break;
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

void MainWindow::receiveShip()
{
    if (serverShip->getResult() == "ok") {
        status = Status::load;
        showMessage("Ваши корабли успешно размещены");
    } else {
        status = Status::ship;
        showMessage("Ошибка размещения кораблей " + serverShip->getError());
    }
}

void MainWindow::prepare()
{
    status = Status::init;
    timer->start(1000);
}

void MainWindow::doInit()
{
    showMessage("Подготовка экрана");
    ui->pictureMain->setPixmap(QPixmap(":/start.png"));
    status = Status::wait;
}

void MainWindow::doWait()
{
    serverInfo->request();
}

void MainWindow::doShip()
{
    int oceanCount  {serverInfo->getOceans()};
    int shipCount   {serverInfo->getShips()};

    oceans.clear();

    for (int i{}; i < oceanCount; ++i) {
        oceans.push_back(SeaBattleClient::Ocean{i});
        oceans[i].create(myOcean == i ? shipCount : 0);
    }

    serverShip->request(myOcean, oceans[myOcean].getMap().getShips());
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
