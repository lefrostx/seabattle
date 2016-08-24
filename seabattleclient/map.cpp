#include "map.h"
#include "mathematic.h"

void SeaBattleClient::Map::create()
{
    for (unsigned row = 0; row < ships.size1(); ++row)
        for (unsigned col = 0; col < ships.size2(); ++col)
            ships(row, col) = EnumShips::none;

    for (unsigned row = 0; row < fires.size1(); ++row)
        for (unsigned col = 0; col < fires.size2(); ++col)
            fires(row, col) = EnumFires::none;

    shipsTotal = shipsKill = shipsRest = 0;
}

bool SeaBattleClient::Map::placeShip()
{
    int loop = 100 * mapSize;

    while(--loop > 0) {
        int row = ClarensMath::randomInRange(0, mapSize - 1);
        int col = ClarensMath::randomInRange(0, mapSize - 1);
        if (placeShip(row, col))
            return true;
    }

    return false;
}

bool SeaBattleClient::Map::placeShip(int shipRow, int shipCol)
{
    for (int row{shipRow - 1}; row <= shipRow + 1; ++row)
        for (int col{shipCol - 1}; col <= shipCol + 1; ++col)
            if (onMap(row, col))
                if (ships(row, col) != EnumShips::none)
                    return false;

    ships(shipRow, shipCol) = EnumShips::ship;
    ++shipsTotal;
    ++shipsRest;

    return true;
}

bool SeaBattleClient::Map::onMap(int row, int col)
{
    return row >= 0 && row < mapSize && col >= 0 && col < mapSize;
}

SeaBattleClient::EnumIcons SeaBattleClient::Map::icon(int row, int col)
{
    if (!onMap(row, col))
        return EnumIcons::unkn;

    switch(fires(row, col)) {
    case EnumFires::fail: return EnumIcons::fail;
    case EnumFires::kill: return EnumIcons::kill;
    case EnumFires::fire: return EnumIcons::fire;
    case EnumFires::none:
        switch(ships(row, col)) {
        case EnumShips::none: return EnumIcons::none;
        case EnumShips::ship: return EnumIcons::ship;
        case EnumShips::unkn: return EnumIcons::unkn;
        }
        break;
    }

    return EnumIcons::unkn;
}

void SeaBattleClient::Map::fire(int row, int col)
{
    if (!onMap(row, col))
        return;

    fires(row, col) = EnumFires::fire;
}

void SeaBattleClient::Map::kill(int row, int col)
{
    if (!onMap(row, col))
        return;

    fires(row, col) = EnumFires::kill;
}

void SeaBattleClient::Map::fail(int row, int col)
{
    if (!onMap(row, col))
        return;

    fires(row, col) = EnumFires::fail;
}
