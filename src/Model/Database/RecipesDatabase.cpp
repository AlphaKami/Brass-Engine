#include "RecipesDatabase.h"

#include <fstream>
#include <stdexcept>

void RecipesDatabase::loadFromFile(const std::string &path, const ResourcesDatabase &resourceDb, const MachineryDatabase &machineDb) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("[RecipesDatabase] Cannot open '" + path + "'");
    }

    nlohmann::json data;
    try {
        file >> data;
    } catch (const nlohmann::json::parse_error& e) {
        throw std::runtime_error("[RecipesDatabase] JSON parsing error '" + path + "': " + e.what());
    }

    if (!data.contains("recipes")) return;

    for (const auto& entry : data["recipes"]) {
        Recipe recipe;
        recipe.id = static_cast<RecipeId>(recipes.size());
        recipe.name = entry.at("id").get<std::string>();
        recipe.displayName = entry.at("displayName").get<std::string>();

        auto machineName = entry.at("machine").get<std::string>();
        recipe.machineId = machineDb.getMachineId(machineName);

        recipe.processingTime = entry.at("processingTime").get<float>();

        recipe.inputs = parseAmounts(entry.at("inputs"), resourceDb);
        recipe.outputs = parseAmounts(entry.at("outputs"), resourceDb);

        if (entry.contains("byproducts")) {
            recipe.byproducts = parseByproducts(entry["byproducts"], recipe.outputs, resourceDb);
        }

        validateRecipeAgainstMachine(recipe, machineDb.getMachine(recipe.machineId));

        if (this->recipeNameToId.contains(recipe.name)) {
            throw std::runtime_error("[RecipesDatabase] Duplicated recipe '" + recipe.name + "' in '" + path + "'");
        }

        this->recipeNameToId[recipe.name] = recipe.id;
        this->recipes.push_back(std::move(recipe));
    }
}

RecipeId RecipesDatabase::getRecipeId(const std::string &name) const {
    const auto it = this->recipeNameToId.find(name);
    if (it == this->recipeNameToId.end()) {
        throw std::runtime_error("[RecipesDatabase] recipe '" + name + "' not found in database");
    }
    return it->second;
}

ResourceRef RecipesDatabase::resolveResource(const nlohmann::json &entry, const ResourcesDatabase &resourceDb) {
    const auto typeStr = entry.at("type").get<std::string>();
    const auto name = entry.at("resource").get<std::string>();

    if (typeStr == "item")  return ResourceRef{ResourceType::Item,  resourceDb.getItemId(name)};
    if (typeStr == "fluid") return ResourceRef{ResourceType::Fluid, resourceDb.getFluidId(name)};
    if (typeStr == "gas")   return ResourceRef{ResourceType::Gas,   resourceDb.getGasId(name)};

    throw std::runtime_error("[RecipesDatabase] Unknown resource type '" + typeStr + "'");
}

std::vector<ResourceAmount> RecipesDatabase::parseAmounts(const nlohmann::json &array, const ResourcesDatabase &resourceDb) {
    std::vector<ResourceAmount> result;
    for (const auto& entry : array) {
        ResourceAmount ra{};
        ra.resource = resolveResource(entry, resourceDb);
        ra.amount = entry.at("amount").get<float>();
        result.push_back(ra);
    }
    return result;
}

std::vector<ByproductAmount> RecipesDatabase::parseByproducts(const nlohmann::json& array, const std::vector<ResourceAmount>& outputs, const ResourcesDatabase& resourceDb) {
    std::vector<ByproductAmount> result;
    for (const auto& entry : array) {
        ByproductAmount ba{};
        ba.resource = resolveResource(entry, resourceDb);
        ba.amount = entry.at("amount").get<float>();
        ba.probability = entry.at("probability").get<float>();
        auto associatedName = entry.at("associatedOutput").get<std::string>();
        ba.associatedOutputIndex = findOutputIndexByName(associatedName, outputs, resourceDb);
        result.push_back(ba);
    }
    return result;
}

size_t RecipesDatabase::findOutputIndexByName(const std::string &name, const std::vector<ResourceAmount> &outputs, const ResourcesDatabase &resourceDb) {
    ResourceRef ref{};
    bool resolved = false;

    if (auto id = resourceDb.tryGetItemId(name)) {
        ref = ResourceRef{ResourceType::Item, *id};
        resolved = true;
    } else if (auto id = resourceDb.tryGetFluidId(name)) {
        ref = ResourceRef{ResourceType::Fluid, *id};
        resolved = true;
    } else if (auto id = resourceDb.tryGetGasId(name)) {
        ref = ResourceRef{ResourceType::Gas, *id};
        resolved = true;
    }

    if (!resolved) {
        throw std::runtime_error("[RecipesDatabase] associatedOutput '" + name + "' is not a valid resource");
    }

    for (size_t i = 0; i < outputs.size(); ++i) {
        if (outputs[i].resource.type == ref.type && outputs[i].resource.id == ref.id) {
            return i;
        }
    }

    throw std::runtime_error("[RecipesDatabase] associatedOutput '" + name + "' does not correspond to a valid resource");

}

size_t RecipesDatabase::countPortsByType(const std::vector<MachinePort> &ports, ResourceType type) {
    return std::ranges::count_if(ports, [type](const MachinePort& p) { return p.acceptedType == type; });
}

size_t RecipesDatabase::countAmountsByType(const std::vector<ResourceAmount> &amounts, ResourceType type) {
    return std::ranges::count_if(amounts, [type](const ResourceAmount& a) { return a.resource.type == type; });
}

void RecipesDatabase::validateRecipeAgainstMachine(const Recipe &recipe, const Machine &machine) {
    for (ResourceType type : {ResourceType::Item, ResourceType::Fluid, ResourceType::Gas}) {
        const size_t neededInputs = countAmountsByType(recipe.inputs, type);
        const size_t availableInputs = countPortsByType(machine.inputs, type);
        if (neededInputs > availableInputs) {
            throw std::runtime_error("Recipe '" + recipe.name + "': requires " +
                std::to_string(neededInputs) + " input ports of type '" + std::to_string(static_cast<int>(type)) +
                "' but machine '" + machine.name + "' has only " + std::to_string(availableInputs));
        }

        const size_t neededOutputs = countAmountsByType(recipe.outputs, type);
        const size_t availableOutputs = countPortsByType(machine.outputs, type);
        if (neededOutputs > availableOutputs) {
            throw std::runtime_error("Recipe '" + recipe.name + "': requires " +
                std::to_string(neededOutputs) + " output ports of type '" + std::to_string(static_cast<int>(type)) +
                "' but machine '" + machine.name + "' has only " + std::to_string(availableOutputs));
        }
    }
}
