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

    connect(serverFire,     &ServerFire::fireReceived,
            this,           &MainWindow::receiveFire);

    connect(serverGame,     &ServerGame::gameReceived,
            this,           &MainWindow::receiveGame);

    SeaBattleClient::Box::pictureMain = ui->pictureMain;
    SeaBattleClient::Box::boxClicked = [this](int ocean, int row, int col){return funcBoxClicked(ocean, row, col);};

    prepare();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::funcBoxClicked(int place, int row, int col)
{
    if (place < 0 || place >= static_cast<int>(oceans.size()))
            return;

    serverFire->request(myOcean, place, row, col);
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
    QString serverStatus{serverInfo->getStatus()};
    int freeOcean{serverInfo->getFreeOcean()};

    if (serverStatus == "wait") {
        if (freeOcean >= 0) {
            int freeOcean{serverInfo->getFreeOcean()};
            if (status == Status::load) {
                showMessage("Ожидание океана № " + QString::number(freeOcean));
                return;
            }
            myOcean = freeOcean;
            ui->pictureMain->clear();
            showMessage("Получен океан № " + QString::number(freeOcean));
            status = Status::ship;
        } else {
            showMessage("Нет свободных океанов");
        }
    } else if (serverStatus == "play") {
        if (status == Status::load) {
            status = Status::play;
            showMessage("Игра");
        } else {
            showMessage("Игра уже идет, дождитесь окончания");
        }
    } else if (serverStatus == "stop") {

        if (status == Status::play) {
            showMessage("Игра завершена. Победил океан № " +
                        QString::number(serverInfo->getWinOcean()));
        } else {
            showMessage("Игра завершена, дождитесь начала новой игры");
        }
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

void MainWindow::receiveFire(int /*fromOcean*/, int toOcean, int toRow, int toCol)
{
    if (serverFire->getResult() == "ok") {
        showMessage("Выстрел по океану № " + QString::number(toOcean) +
                    " на клетку [" + QString::number(toRow) + ", " +
                    QString::number(toCol) + "]");
        oceans[toOcean].fire(toRow, toCol);
    }
}

void MainWindow::receiveGame()
{
    for (const auto & x : serverGame->getActions()) {

        if (x.action == "fire")
            oceans[x.toOcean].fire(x.toRow, x.toCol);

        else if (x.action == "kill")
            oceans[x.toOcean].kill(x.toRow, x.toCol);

        else if (x.action == "fail")
            oceans[x.toOcean].fail(x.toRow, x.toCol);
    }
    if (serverGame->getStatus() == "stop")
        serverInfo->request();
}

void MainWindow::prepare()
{
    status = Status::init;
    doInit();
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
    serverInfo->request();
}

void MainWindow::doPlay()
{
    serverGame->request();
}

void MainWindow::doStop()
{
    serverInfo->request();
}

void MainWindow::showMessage(const QString &message)
{
    ui->labelMessage->setText(message);
}
