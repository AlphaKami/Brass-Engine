#include "MarkerRegistry.h"

#include <fstream>
#include <stdexcept>
#include <nlohmann/json.hpp>

MarkerRegistry::MarkerRegistry() {
    defaultMarkId = resolve(DefaultMarkName);
}

void MarkerRegistry::loadFromFile(const std::string &path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("[MarkerRegistry] Cannot open '" + path + "'");
    }

    nlohmann::json data;
    try {
        file >> data;
    } catch (const nlohmann::json::parse_error& e) {
        throw std::runtime_error("[MarkerRegistry] JSON parsing error in '" + path + "': " + e.what());
    }

    if (!data.contains("markers")) return;

    for (const auto& entry : data.at("markers")) {
        resolve(entry.get<std::string>());
    }
}

MarkerId MarkerRegistry::resolve(const std::string& name) {
    const auto it = nameToId.find(name);
    if (it != nameToId.end()) return it->second;

    const MarkerId id = static_cast<MarkerId>(idToName.size());
    nameToId.emplace(name, id);
    idToName.push_back(name);
    return id;
}

MarkerId MarkerRegistry::get(const std::string &name) const {
    const auto it = nameToId.find(name);
    if (it == nameToId.end())
        throw std::runtime_error("[MarkerRegistry] unknown marker '" + name + "' (declare it in the markers file)");
    return it->second;
}

const std::string& MarkerRegistry::nameOf(const MarkerId id) const {
    if (id >= idToName.size())
        throw std::runtime_error("[TagRegistry] unknown TagId " + std::to_string(id));
    return idToName[id];
}
