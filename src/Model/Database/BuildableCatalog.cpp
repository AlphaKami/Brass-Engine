#include "BuildableCatalog.h"

BuildableId BuildableCatalog::registerBuildable(BuildableDef def) {
    if (nameToId.contains(def.name))
        throw std::runtime_error("[BuildableCatalog] duplicate buildable name: " + def.name);
    def.id = static_cast<BuildableId>(defs.size());
    nameToId.emplace(def.name, def.id);
    defs.push_back(std::move(def));
    return defs.back().id;
}

const BuildableDef & BuildableCatalog::get(const BuildableId id) const {
    return defs.at(id);
}

BuildableId BuildableCatalog::getIdByName(const std::string &name) const {
    const auto it = nameToId.find(name);
    if (it == nameToId.end()) throw std::runtime_error("[BuildableCatalog] unknown buildable name: " + name);
    return it->second;
}
