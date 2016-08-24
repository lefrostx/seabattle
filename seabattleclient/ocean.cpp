#include "ocean.h"

SeaBattleClient::Ocean::Ocean(int oceanPlace, const QString &oceanTitle) :
    place{oceanPlace},
    title{oceanTitle}
{
}

void SeaBattleClient::Ocean::create(int shipCount)
{
    map.create();

    for (int i{}; i < shipCount; ++i)
        map.placeShip();

    createBoxes();
}

void SeaBattleClient::Ocean::fire(int row, int col)
{
    map.fire(row, col);
    changeBox(row, col);
}

void SeaBattleClient::Ocean::kill(int row, int col)
{
    map.kill(row, col);
    changeBox(row, col);
}

void SeaBattleClient::Ocean::fail(int row, int col)
{
    map.fail(row, col);
    changeBox(row, col);
}

void SeaBattleClient::Ocean::createBoxes()
{
    for (unsigned row = 0; row < box.size1(); ++row)
        for (unsigned col = 0; col < box.size2(); ++col)
            createBox(row, col);
}

void SeaBattleClient::Ocean::createBox(int row, int col)
{
    box(row, col) = Box::create(map.icon(row, col));
    box(row, col)->init(place, row, col);
    box(row, col)->show();
}

void SeaBattleClient::Ocean::changeBox(int row, int col)
{
    box(row, col)->setParent(0);
    box(row, col)->deleteLater();

    createBox(row, col);
}
