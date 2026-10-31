#ifndef IRONBRASS_MACHINERYDATABASE_H
#define IRONBRASS_MACHINERYDATABASE_H

#include <unordered_map>
#include "Machine.h"


class MachineryDatabase {
public:
    void loadFromFile(const std::string& path);

    [[nodiscard]] const Machine& getMachine(const MachineId id) const { return machines.at(id); }
    [[nodiscard]] MachineId getMachineId(const std::string& name) const;

    [[nodiscard]] size_t machineCount() const { return machines.size(); }

private:
    std::vector<Machine> machines;
    std::unordered_map<std::string, MachineId> machineNameToId;
};


#endif //IRONBRASS_MACHINERYDATABASE_H
