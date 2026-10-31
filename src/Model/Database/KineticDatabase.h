#ifndef IRONBRASS_KINETICDATABASE_H
#define IRONBRASS_KINETICDATABASE_H

#include "BuildableCatalog.h"
#include "../Behaviours/Kinetic/KineticDef.h"
#include "../BuildSystem/MarkerRegistry.h"

#include <cstdint>
#include <string>
#include <unordered_map>


/**
 * Kinetic data relative to kinetics such as gears, shafts, motors, etc.
 */
class KineticDatabase {
public:
    /**
     * @requires `catalog` and `markers` already loaded: the file refers to both by name.
     * @throws std::runtime_error on any malformed entry, unknown buildable/marker/orientation or duplicate
     */
    void loadFromFile(const std::string& path, const BuildableCatalog& catalog, const MarkerRegistry& markers);

    /**
     * @return the definition for that buildable orientation, or nullptr if it has no kinetic behavior.
     * The pointer stays valid for the lifetime of the database as long as nothing is loaded afterward, so entities can keep it.
     */
    [[nodiscard]] const KineticDef* find(BuildableId id, std::uint8_t orientation) const;

    [[nodiscard]] size_t size() const { return defs.size(); }
    [[nodiscard]] int maxLinkReach() const { return linkReach; }
private:
    std::unordered_map<std::uint64_t, KineticDef> defs;
    int linkReach = 0;

    static std::uint64_t keyOf(const BuildableId id, const std::uint8_t orientation) {
        return (static_cast<std::uint64_t>(id) << 8) | orientation;
    }
};


#endif //IRONBRASS_KINETICDATABASE_H
