#ifndef IRONBRASS_PRODUCTIONSYSTEM_H
#define IRONBRASS_PRODUCTIONSYSTEM_H

#include "../Components/MachineInstance.h"
#include "../../Database/RNG.h"

inline bool processMachineInstance(MachineInstance& machine, const Recipe& recipe, const float& dt) {
    for (size_t i = 0; i < machine.inputBuffers.size(); ++i) {
        if (machine.inputBuffers.at(i).amount < recipe.inputs.at(i).amount) {
            return false;
        }
    }

    for (size_t i = 0; i < machine.outputBuffers.size(); ++i) {
        float spaceLeft = machine.outputBuffers.at(i).capacity - machine.outputBuffers.at(i).amount;
        if (spaceLeft < recipe.outputs.at(i).amount) {
            return false;
        }
    }

    machine.progress += dt;
    if (machine.progress < recipe.processingTime) {
        return false;
    }

    machine.progress -= recipe.processingTime;

    for (size_t i = 0; i < machine.inputBuffers.size(); ++i) {
        machine.inputBuffers.at(i).amount -= recipe.inputs.at(i).amount;
    }
    for (size_t i = 0; i < machine.outputBuffers.size(); ++i) {
        machine.outputBuffers.at(i).amount += recipe.outputs.at(i).amount;
    }

    for (size_t i = 0; i < machine.byproductBuffers.size(); ++i) {
        const auto& bpRecipe = recipe.byproducts.at(i);
        if (RNG::rollProbability(bpRecipe.probability)) {
            auto& buf = machine.byproductBuffers.at(i);
            float spaceLeft = buf.capacity - buf.amount;
            buf.amount += std::min(bpRecipe.amount, spaceLeft);
        }
    }

    return true;
}

#endif //IRONBRASS_PRODUCTIONSYSTEM_H
