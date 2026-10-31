#ifndef IRONBRASS_RNG_H
#define IRONBRASS_RNG_H

#include <random>

namespace RNG {
    inline thread_local std::mt19937 generator{std::random_device{}()};

    inline bool rollProbability(const float p) {
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        return dist(generator) < p;
    }
}

#endif //IRONBRASS_RNG_H
