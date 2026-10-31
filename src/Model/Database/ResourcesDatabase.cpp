#include "ResourcesDatabase.h"

#include <fstream>

void ResourcesDatabase::loadFromFile(const std::string &path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("[ResourceDatabase] Cannot open '" + path + "'");
    }

    nlohmann::json data;
    try {
        file >> data;
    } catch (const nlohmann::json::parse_error& e) {
        throw std::runtime_error("[ResourceDatabase] JSON parsing error in '" + path + "': " + e.what());
    }

    // --- Items ---
    if (data.contains("items")) {
        for (const auto& entry : data["items"]) {
            Item item;
            item.id = static_cast<ItemId>(this->items.size());
            item.name = entry.at("id").get<std::string>();
            item.displayName = entry.at("displayName").get<std::string>();
            item.weight = entry.at("weight").get<float>();

            if (this->itemNameToId.contains(item.name)) {
                throw std::runtime_error("[ResourceDatabase] Duplicated item '" + item.name + "' in '" + path + "'");
            }

            this->itemNameToId[item.name] = item.id;
            this->items.push_back(std::move(item));
        }
    }

    // --- Fluids ---
    if (data.contains("fluids")) {
        for (const auto& entry : data["fluids"]) {
            Fluid fluid;
            fluid.id = static_cast<FluidId>(this->fluids.size());
            fluid.name = entry.at("id").get<std::string>();
            fluid.displayName = entry.at("displayName").get<std::string>();
            fluid.density = entry.at("density").get<float>();

            if (this->fluidNameToId.contains(fluid.name)) {
                throw std::runtime_error("[ResourceDatabase] Duplicated fluid '" + fluid.name + "' in '" + path + "'");
            }

            this->fluidNameToId[fluid.name] = fluid.id;
            this->fluids.push_back(std::move(fluid));
        }
    }

    // --- Gases ---
    if (data.contains("gases")) {
        for (const auto& entry : data["gases"]) {
            Gas gas;
            gas.id = static_cast<GasId>(this->gases.size());
            gas.name = entry.at("id").get<std::string>();
            gas.displayName = entry.at("displayName").get<std::string>();
            gas.pressure = entry.at("pressure").get<float>();

            if (this->gasNameToId.contains(gas.name)) {
                throw std::runtime_error("[ResourceDatabase] Duplicated gas '" + gas.name + "' in '" + path + "'");
            }

            this->gasNameToId[gas.name] = gas.id;
            this->gases.push_back(std::move(gas));
        }
    }
}

ItemId ResourcesDatabase::getItemId(const std::string &name) const {
    const auto it = this->itemNameToId.find(name);
    if (it == this->itemNameToId.end()) {
        throw std::runtime_error("[ResourceDatabase] Item '" + name + "' not found in database");
    }
    return it->second;
}

FluidId ResourcesDatabase::getFluidId(const std::string &name) const {
    const auto it = this->fluidNameToId.find(name);
    if (it == this->fluidNameToId.end()) {
        throw std::runtime_error("[ResourceDatabase] fluid '" + name + "' not found in database");
    }
    return it->second;
}

GasId ResourcesDatabase::getGasId(const std::string &name) const {
    const auto it = this->gasNameToId.find(name);
    if (it == this->gasNameToId.end()) {
        throw std::runtime_error("[ResourceDatabase] gas '" + name + "' not found in database");
    }
    return it->second;
}

std::optional<ItemId> ResourcesDatabase::tryGetItemId(const std::string &name) const {
    const auto it = this->itemNameToId.find(name);
    return it != this->itemNameToId.end() ? std::optional{it->second} : std::nullopt;
}

std::optional<FluidId> ResourcesDatabase::tryGetFluidId(const std::string &name) const {
    const auto it = this->fluidNameToId.find(name);
    return it != this->fluidNameToId.end() ? std::optional{it->second} : std::nullopt;
}

std::optional<GasId> ResourcesDatabase::tryGetGasId(const std::string &name) const {
    const auto it = this->gasNameToId.find(name);
    return it != this->gasNameToId.end() ? std::optional{it->second} : std::nullopt;
}
