#ifndef CLARENS_MATHEMATIC_H
#define CLARENS_MATHEMATIC_H

#include <random>
#include <chrono>

namespace ClarensMath {

    inline int randomInRange(int min, int max)
    {
        auto seed = std::chrono::system_clock::now().time_since_epoch().count();
        static std::default_random_engine generator{seed};

        return std::uniform_int_distribution<int>{min, max}(generator);
    }
}

#endif // CLARENS_MATHEMATIC_H

