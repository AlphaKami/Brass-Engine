#pragma once

#include <string>
#include <vector>
#include <cstdint>

#include "ResourceRef.h"
#include "Machine.h"

using RecipeId = std::uint32_t;

struct ResourceAmount {
    ResourceRef resource;
    float amount;
};

struct ByproductAmount {
    ResourceRef resource;
    float amount;
    float probability;
    size_t associatedOutputIndex;
};

struct Recipe {
    RecipeId id;
    std::string name;
    std::string displayName;
    MachineId machineId;

    std::vector<ResourceAmount> inputs;
    std::vector<ResourceAmount> outputs;
    std::vector<ByproductAmount> byproducts;

    float processingTime;
};