#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

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

private:
    void prepare();
    void doInit();
    void doWait();
    void doShip();
    void doLoad();
    void doPlay();
    void doStop();

    Ui::MainWindow *ui;
    QTimer* timer;
    Status status{Status::init};
};

#endif // MAINWINDOW_H
