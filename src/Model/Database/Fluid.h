#ifndef IRONBRASS_FLUID_H
#define IRONBRASS_FLUID_H

#include <cstdint>
#include <string>

using FluidId = std::uint32_t;

struct Fluid {
    FluidId id;
    std::string name;
    std::string displayName;
    float density;
};

#endif //IRONBRASS_FLUID_H
