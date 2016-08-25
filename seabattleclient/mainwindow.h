#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <vector>
#include "server.h"
#include "ocean.h"

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
    void funcBoxClicked(int place, int row, int col);

public slots:
    void updateStatus();
    void receiveInfo();
    void receiveShip();
    void receiveFire(int fromOcean, int toOcean, int toRow, int toCol);
    void receiveGame();

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

    std::vector<SeaBattleClient::Ocean> oceans;

    Status status{Status::init};
    int myOcean{-1};
};

#endif // MAINWINDOW_H
