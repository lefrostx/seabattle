#ifndef SEABATTLECLIENT_OCEAN_H
#define SEABATTLECLIENT_OCEAN_H

#include <QString>
#include <boost/numeric/ublas/matrix.hpp>
#include "map.h"
#include "box.h"

using boost::numeric::ublas::matrix;

namespace SeaBattleClient {

    class Ocean {
    public:
        explicit Ocean(int oceanPlace);
        void create(int shipCount);

        void fire(int row, int col);
        void kill(int row, int col);
        void fail(int row, int col);
        const Map& getMap() const {return map;}

    private:
        void createBoxes();
        void createBox(int row, int col);
        void changeBox(int row, int col);
        int place;
        Map map;
        matrix<Box*> box {Map::mapSize, Map::mapSize};
    };
}

#endif // SEABATTLECLIENT_OCEAN_H
