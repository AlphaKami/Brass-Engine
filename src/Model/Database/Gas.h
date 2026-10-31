#ifndef IRONBRASS_GAS_H
#define IRONBRASS_GAS_H

#include <cstdint>
#include <string>

using GasId = std::uint32_t;

struct Gas {
    GasId id;
    std::string name;
    std::string displayName;
    float pressure;
};

#endif //IRONBRASS_GAS_H
