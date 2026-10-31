#ifndef IRONBRASS_MACHINE_H
#define IRONBRASS_MACHINE_H

#include "ResourceRef.h"
#include <string>
#include <vector>

struct TilePos { int x, y, z; };
struct MachineSize { int xSize, ySize, zSize; };

struct MachinePort {
    ResourceType acceptedType;
    TilePos position;
};

using MachineId = std::uint32_t;

struct Machine {
    MachineId id;
    std::string name;
    std::string displayName;

    MachineSize size;
    std::vector<MachinePort> inputs;
    std::vector<MachinePort> outputs;
};

namespace BufferCapacity {
    constexpr float Item  = 20.0f;
    constexpr float Fluid = 50.0f;  // liters
    constexpr float Gas   = 100.0f; // liters
}

#endif //IRONBRASS_MACHINE_H
