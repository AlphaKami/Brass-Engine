#ifndef IRONBRASS_BUILDABLECATALOG_H
#define IRONBRASS_BUILDABLECATALOG_H

#include "Buildable.h"
#include <vector>

class BuildableCatalog {
public:
    BuildableId registerBuildable(BuildableDef def);
    [[nodiscard]] const BuildableDef& get(BuildableId id) const;
    [[nodiscard]] size_t buildableCount() const { return defs.size(); }

    BuildableId getIdByName(const std::string& name) const;

private:
    std::vector<BuildableDef> defs;
    std::unordered_map<std::string, BuildableId> nameToId;
};


#endif //IRONBRASS_BUILDABLECATALOG_H
