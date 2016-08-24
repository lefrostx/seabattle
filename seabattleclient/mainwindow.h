#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "server.h"

namespace Ui {
class MainWindow;
}

enum class Status {
    init,
    wait,
    ship,
    load,
    play,
    stop
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void updateStatus();
    void receiveInfo();

private:
    void prepare();
    void doInit();
    void doWait();
    void doShip();
    void doLoad();
    void doPlay();
    void doStop();
    void showMessage(const QString & message);

    Ui::MainWindow *ui;
    QTimer *timer;

    SeaBattleClient::ServerInfo *serverInfo;
    SeaBattleClient::ServerShip *serverShip;
    SeaBattleClient::ServerFire *serverFire;
    SeaBattleClient::ServerGame *serverGame;

    Status status{Status::init};
    int myOcean{-1};
};

#endif // MAINWINDOW_H
