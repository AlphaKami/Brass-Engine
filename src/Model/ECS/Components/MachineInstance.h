#ifndef IRONBRASS_MACHINEINSTANCE_H
#define IRONBRASS_MACHINEINSTANCE_H

#include "../../Database/RecipesDatabase.h"
#include "../../Database/Machine.h"
#include "../../Database/Recipe.h"
#include "../../Database/ResourceRef.h"



struct ResourceBuffer {
    ResourceRef resource;
    float amount = 0.0f;
    float capacity = 0.0f;
};

struct MachineInstance {
    MachineId machineId{};
    RecipeId recipeId{};

    std::vector<ResourceBuffer> inputBuffers;
    std::vector<ResourceBuffer> outputBuffers;
    std::vector<ResourceBuffer> byproductBuffers;

    float progress = 0.0f;
};

/**
 * Find the buffer capacity of the resource type.
 * @return BufferCapacity for the type or 0.0f if invalid type.
 */
inline float bufferCapacityFor(const ResourceType type) {
    switch (type) {
        case ResourceType::Item:  return BufferCapacity::Item;
        case ResourceType::Fluid: return BufferCapacity::Fluid;
        case ResourceType::Gas:   return BufferCapacity::Gas;
    }
    return 0.0f;
}

inline MachineInstance createMachineInstance(const MachineId machineId, const RecipeId recipeId, const RecipesDatabase& recipeDb) {
    const Recipe& recipe = recipeDb.getRecipe(recipeId);

    MachineInstance instance;
    instance.machineId = machineId;
    instance.recipeId = recipeId;

    for (const auto& in : recipe.inputs) {
        instance.inputBuffers.push_back(ResourceBuffer{in.resource, 0.0f, bufferCapacityFor(in.resource.type)});
    }
    for (const auto& out : recipe.outputs) {
        instance.outputBuffers.push_back(ResourceBuffer{out.resource, 0.0f, bufferCapacityFor(out.resource.type)});
    }
    for (const auto& bp : recipe.byproducts) {
        instance.byproductBuffers.push_back(ResourceBuffer{bp.resource, 0.0f, bufferCapacityFor(bp.resource.type)});
    }

    return instance;
}

inline bool addToBuffer(ResourceBuffer& buffer, const ResourceRef resource, const float amount) {
    if (buffer.resource.type != resource.type || buffer.resource.id != resource.id) {
        return false;
    }
    const float spaceLeft = buffer.capacity - buffer.amount;
    const float toAdd = std::min(amount, spaceLeft);
    buffer.amount += toAdd;
    return toAdd > 0.0f;
}

#endif //IRONBRASS_MACHINEINSTANCE_H
