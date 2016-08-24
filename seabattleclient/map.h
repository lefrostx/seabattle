#ifndef SEABATTLECLIENT_MAP_H
#define SEABATTLECLIENT_MAP_H

#include <boost/numeric/ublas/matrix.hpp>
using boost::numeric::ublas::matrix;

namespace SeaBattleClient {

    enum class EnumShips {
        unkn,
        none,
        ship
    };

    enum class EnumFires {
        none,
        kill,
        fail,
        fire
    };

    enum class EnumIcons {
        unkn,
        none,
        ship,
        kill,
        fail,
        fire
    };

    class Map {
    public:
        static const int mapSize = 5;
        void create();
        bool placeShip();
        bool placeShip(int shipRow, int shipCol);
        bool onMap(int row, int col);
        EnumIcons icon(int row, int col);
        void fire(int row, int col);
        void kill(int row, int col);
        void fail(int row, int col);
        int getShipsTotal() const {return shipsTotal;}
        int getShipsKill() const {return shipsKill;}
        int getShipsRest() const {return shipsRest;}
        const matrix<EnumShips> & getShips() const {return ships;}

    private:
        matrix<EnumShips> ships {mapSize, mapSize};
        matrix<EnumFires> fires {mapSize, mapSize};
        int shipsTotal{};
        int shipsKill{};
        int shipsRest{};
    };
}

#endif // SEABATTLECLIENT_MAP_H
