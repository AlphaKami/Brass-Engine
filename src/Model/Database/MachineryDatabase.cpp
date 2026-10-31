#include "MachineryDatabase.h"

#include <fstream>
#include <stdexcept>

namespace {
    ResourceType parseResourceType(const std::string& typeStr) {
        if (typeStr == "item")  return ResourceType::Item;
        if (typeStr == "fluid") return ResourceType::Fluid;
        if (typeStr == "gas")   return ResourceType::Gas;
        throw std::runtime_error("[MachineryDatabase] Unknown resource type '" + typeStr + "'");
    }

    std::vector<MachinePort> parsePorts(const nlohmann::json& portsArray) {
        std::vector<MachinePort> ports;
        for (const auto& entry : portsArray) {
            MachinePort port{};
            port.acceptedType = parseResourceType(entry.at("type").get<std::string>());
            port.position.x = entry.at("position").at("x").get<int>();
            port.position.y = entry.at("position").at("y").get<int>();
            port.position.z = entry.at("position").at("z").get<int>();
            ports.push_back(port);
        }
        return ports;
    }
}

void MachineryDatabase::loadFromFile(const std::string &path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("[MachineryDatabase] Cannot open '" + path + "'");
    }

    nlohmann::json data;
    try {
        file >> data;
    } catch (const nlohmann::json::parse_error& e) {
        throw std::runtime_error("[MachineryDatabase] JSON parsing error '" + path + "': " + e.what());
    }

    if (!data.contains("machines")) return;

    for (const auto& entry : data["machines"]) {
        Machine machine;
        machine.id = static_cast<MachineId>(this->machines.size());
        machine.name = entry.at("id").get<std::string>();
        machine.displayName = entry.at("displayName").get<std::string>();

        machine.size.xSize = entry.at("size").at("xSize").get<int>();
        machine.size.ySize = entry.at("size").at("ySize").get<int>();
        machine.size.zSize = entry.at("size").at("zSize").get<int>();

        machine.inputs = parsePorts(entry.at("inputs"));
        machine.outputs = parsePorts(entry.at("outputs"));

        if (this->machineNameToId.contains(machine.name)) {
            throw std::runtime_error("[MachineryDatabase] Duplicated machine '" + machine.name + "' in '" + path + "'");
        }

        this->machineNameToId[machine.name] = machine.id;
        this->machines.push_back(std::move(machine));
    }
}

MachineId MachineryDatabase::getMachineId(const std::string &name) const {
    const auto it = this->machineNameToId.find(name);
    if (it == this->machineNameToId.end()) {
        throw std::runtime_error("[MachineryDatabase] Cannot find machine '" + name + "' in database");
    }
    return it->second;
}
