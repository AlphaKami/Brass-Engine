#ifndef IRONBRASS_SIMULATION_H
#define IRONBRASS_SIMULATION_H

#include <entt/entt.hpp>
#include "ThreadPool.h"
#include "Database/BuildableCatalog.h"
#include "Database/RecipesDatabase.h"
#include "Map/TileGrid.h"
#include "BuildSystem/CellContents.h"
#include "BuildSystem/MarkerRegistry.h"
#include "Database/KineticDatabase.h"
#include "Behaviours/IBehaviourModule.h"
#include "Behaviours/Kinetic/KineticPreview.h"

#include <optional>
#include <memory>
#include <vector>
class ResourceManager;

//TODO: Simulation grew out of scope in a god-object. Need to be clean later.

class Simulation {
public:
    Simulation();

    void loadDatabases(const std::string& resourcesPath, const std::string& machinesPath, const std::string& recipesPath);
    entt::entity spawnMachine(const std::string& machineName, const std::string& recipeName);

    /// Declares the markers listed in `markersPath`, then loads every buildable defined in `buildablesPath`.
    void loadBuildables(const std::string& markersPath, const std::string& buildablesPath);
    /// Loads the kinetic data (gears, shafts, sources) and enables the kinetic behavior.
    void loadKinetic(const std::string& kineticPath);

    entt::entity spawnBuildable(BuildableId id, uint8_t orientationIndex, const TileCoord& origin);
    void removeBuildable(entt::entity entity);
    void placeTile(BuildableId id, uint8_t orientationIndex, const TileCoord& origin);
    void removeTile(BuildableId id, uint8_t orientationIndex, const TileCoord& origin);

    void tick(const float& dt);

    entt::registry& getRegistry() { return registry; }
    [[nodiscard]] const entt::registry& getRegistry() const { return registry; }

    [[nodiscard]] const ResourcesDatabase& getResourcesDatabase() const { return resourceDb; }
    [[nodiscard]] const MachineryDatabase& getMachineryDatabase() const { return machineDb; }
    [[nodiscard]] const RecipesDatabase& getRecipesDatabase() const { return recipeDb; }

    [[nodiscard]] const OccupancyGrid& getOccupancyGrid() const { return occupancy; }
    [[nodiscard]] const BuildableCatalog& getBuildableCatalog() const { return buildableCatalog; }
    [[nodiscard]] BuildableCatalog& getBuildableCatalog() { return buildableCatalog; }

    [[nodiscard]] MarkerRegistry& getMarkerRegistry() { return markerRegistry; }
    [[nodiscard]] const std::vector<TileSource>& getPlacedTiles() const { return placedTiles; }
    [[nodiscard]] std::optional<KineticPreview> previewKinetic(BuildableId id, uint8_t orientationIndex, const TileCoord& origin) const;
    [[nodiscard]] std::uint64_t getBuildablesVersion() const { return buildablesVersion; }

private:
    entt::registry registry;
    ThreadPool pool;

    ResourcesDatabase resourceDb;
    MachineryDatabase machineDb;
    RecipesDatabase recipeDb;

    BuildableCatalog buildableCatalog;
    MarkerRegistry markerRegistry;
    OccupancyGrid occupancy;
    std::vector<TileSource> placedTiles;
    std::uint64_t buildablesVersion = 0;
    KineticDatabase kineticDb;
    std::vector<std::unique_ptr<IBehaviourModule>> behaviorModules;

    void registerCellPresence(const BuildableOrientation& orientation, const TileCoord& origin, const CellSource& source);
    void unregisterCellPresence(const BuildableOrientation& orientation, const TileCoord& origin, const CellSource& source);
    void registerBehaviourModule(std::unique_ptr<IBehaviourModule> module);

    void runProductionPhase(const float& dt);
    void runKineticPhase(const float& dt);
};


#endif //IRONBRASS_SIMULATION_H
