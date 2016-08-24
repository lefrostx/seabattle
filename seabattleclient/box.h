#ifndef SEABATTLECLIENT_BOX_H
#define SEABATTLECLIENT_BOX_H

#include <QPushButton>
#include <functional>
#include "map.h"

namespace SeaBattleClient {

    class Box : public QPushButton {
        Q_OBJECT
    public:
        explicit Box(QWidget *parent = 0);
        void init(int boxPlace, int boxRow, int boxCol);

        static Box *create(EnumIcons icon);
        static QWidget* pictureMain;
        static std::function <void(int, int, int)> boxClicked;
        static const int iconSize{32};

    public slots:
        void fire();

    private:
        int place{};
        int row{};
        int col{};
    };

    class UnknBox : public Box {
    public:
        explicit UnknBox(QWidget *parent = 0);
    };

    class NoneBox : public Box {
    public:
        explicit NoneBox(QWidget *parent = 0);
    };

    class ShipBox : public Box {
    public:
        explicit ShipBox(QWidget *parent = 0);
    };

    class KillBox : public Box {
    public:
        explicit KillBox(QWidget *parent = 0);
    };

    class FailBox : public Box {
    public:
        explicit FailBox(QWidget *parent = 0);
    };

    class FireBox : public Box {
    public:
        explicit FireBox(QWidget *parent = 0);
    };

}

#endif // SEABATTLECLIENT_BOX_H
