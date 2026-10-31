#include "Simulation.h"

#include "Phase.h"
#include "../Core/ResourceManager.h"
#include "BuildSystem/Predicates/CellPredicates.h"
#include "ECS/Systems/ProductionSystem.h"
#include "../View/ECS/Components/AnimationComponent.h"
#include "BuildSystem/BuildableLoader.h"
#include "Behaviours/Kinetic/KineticModule.h"
#include "Behaviours/Kinetic/KineticDebug.h"
#include "ECS/Systems/KineticSystem.h"
#include "ECS/Components/KineticComponent.h"
#include <algorithm>
#include <iostream>


Simulation::Simulation()
    : pool(std::max(1u, std::thread::hardware_concurrency() > 1 ? std::thread::hardware_concurrency() - 1 : 1))
{
}

void Simulation::loadDatabases(const std::string &resourcesPath, const std::string &machinesPath, const std::string &recipesPath) {
    this->resourceDb.loadFromFile(resourcesPath);
    this->machineDb.loadFromFile(machinesPath);
    this->recipeDb.loadFromFile(recipesPath, this->resourceDb, this->machineDb);
}

entt::entity Simulation::spawnMachine(const std::string &machineName, const std::string &recipeName) {
    const MachineId machineId = this->machineDb.getMachineId(machineName);
    const RecipeId recipeId = this->recipeDb.getRecipeId(recipeName);

    const entt::entity entity = this->registry.create();
    this->registry.emplace<MachineInstance>(entity, createMachineInstance(machineId, recipeId, this->recipeDb));
    return entity;
}

void Simulation::loadBuildables(const std::string &markersPath, const std::string &buildablesPath) {
    this->markerRegistry.loadFromFile(markersPath);
    BuildableLoader loader(this->buildableCatalog, this->markerRegistry);
    loader.loadFromFile(buildablesPath);
}

void Simulation::loadKinetic(const std::string &kineticPath) {
    this->kineticDb.loadFromFile(kineticPath, this->buildableCatalog, this->markerRegistry);
    this->registerBehaviourModule(std::make_unique<KineticModule>(this->kineticDb));
}

void Simulation::registerCellPresence(const BuildableOrientation& orientation, const TileCoord& origin, const CellSource& source) {
    for (const auto& cell : orientation.cells) {
        if (cell.markers.empty()) continue;

        const TileCoord coord = absoluteCell(origin, cell);
        CellContents contents = occupancy.get(coord) ? *occupancy.get(coord) : CellContents{};

        for (const MarkerId marker : cell.markers) {
            contents.markers.push_back(CellMarker{marker, source});
        }

        occupancy.set(coord, std::move(contents));
    }
}

void Simulation::unregisterCellPresence(const BuildableOrientation& orientation, const TileCoord& origin, const CellSource& source) {
    for (const auto& cell : orientation.cells) {
        const TileCoord coord = absoluteCell(origin, cell);
        const CellContents* existing = occupancy.get(coord);
        if (existing == nullptr) continue;

        CellContents contents = *existing;
        std::erase_if(contents.markers, [&source](const CellMarker& m) { return m.source == source; });

        if (contents.isEmpty()) occupancy.remove(coord);
        else occupancy.set(coord, std::move(contents));
    }
}

void Simulation::registerBehaviourModule(std::unique_ptr<IBehaviourModule> module) {
    module->onAttach(this->registry);
    this->behaviorModules.push_back(std::move(module));

}

void Simulation::runKineticPhase(const float &dt) {
    auto* network = this->registry.ctx().find<KineticNetworkState>();
    if (network == nullptr) return; // kinetic behavior not loaded

    if (network->topologyDirty) {
        rebuildKineticLinks(this->registry, this->occupancy);
        network->topologyDirty = false;
        network->speedsDirty = true;
    }

    if (network->speedsDirty) {
        const KineticResolveReport report = resolveKineticNetworks(this->registry);
        network->speedsDirty = false;

        constexpr bool DEBUG_KINETIC = false;
        if (DEBUG_KINETIC) {
            std::cout << "[Kinetic] resolved: " << report.networks << " powered network(s), " << report.runningNodes
                << " running, " << report.idleNodes << " idle, " << report.jammedNodes << " jammed node(s)\n";
            printKineticLinks(this->registry, this->buildableCatalog, std::cout);
        }
    }
    integrateKineticAngles(this->registry, dt);
}

entt::entity Simulation::spawnBuildable(const BuildableId id, const uint8_t orientationIndex, const TileCoord& origin) {
    const BuildableDef& def = buildableCatalog.get(id);
    const BuildableOrientation& orientation = def.orientations.at(orientationIndex);

    const entt::entity entity = registry.create();
    registry.emplace<TileCoord>(entity, origin);
    registry.emplace<Buildable>(entity, Buildable{id, orientationIndex});

    registerCellPresence(orientation, origin, CellSource{entity});
    const Buildable spawned{id, orientationIndex};
    for (const auto& module : behaviorModules) module->onSpawn(registry, entity, spawned);
    ++buildablesVersion;
    return entity;
}

void Simulation::removeBuildable(entt::entity entity) {
    if (!registry.all_of<TileCoord, Buildable>(entity)) return;

    const auto& pos = registry.get<TileCoord>(entity);
    const auto& built = registry.get<Buildable>(entity);
    const BuildableOrientation& orientation = buildableCatalog.get(built.id).orientations.at(built.orientation);

    unregisterCellPresence(orientation, pos, CellSource{entity});
    for (const auto& module : behaviorModules) module->onRemove(registry, entity, built);
    registry.destroy(entity);
    ++buildablesVersion;
}

void Simulation::placeTile(BuildableId id, uint8_t orientationIndex, const TileCoord& origin) {
    const BuildableDef& def = buildableCatalog.get(id);
    const BuildableOrientation& orientation = def.orientations.at(orientationIndex);
    const TileSource source{id, orientationIndex, origin};

    registerCellPresence(orientation, origin, CellSource{source});
    placedTiles.push_back(source);
    ++buildablesVersion;
}

void Simulation::removeTile(BuildableId id, uint8_t orientationIndex, const TileCoord& origin) {
    const BuildableDef& def = buildableCatalog.get(id);
    const BuildableOrientation& orientation = def.orientations.at(orientationIndex);
    const TileSource source{id, orientationIndex, origin};

    unregisterCellPresence(orientation, origin, CellSource{source});
    std::erase(placedTiles, source);
    ++buildablesVersion;
}

void Simulation::tick(const float &dt) {
    this->runProductionPhase(dt);
    this->runKineticPhase(dt);
}

std::optional<KineticPreview> Simulation::previewKinetic(BuildableId id, uint8_t orientationIndex, const TileCoord &origin) const {
    const KineticDef* def = this->kineticDb.find(id, orientationIndex);
    if (def == nullptr) return std::nullopt;

    const BuildableOrientation& orientation = this->buildableCatalog.get(id).orientations.at(orientationIndex);
    return findKineticPreview(this->registry, this->occupancy, this->kineticDb, *def, orientation, origin);
}

void Simulation::runProductionPhase(const float &dt) {
    const auto view = this->registry.view<MachineInstance>();

    std::vector<entt::entity> entities(view.begin(), view.end());

    constexpr size_t BATCH_SIZE = 64; // TODO: Must be profiled later
    const size_t totalEntities = entities.size();
    const size_t numBatches = (totalEntities + BATCH_SIZE - 1) / BATCH_SIZE; // ceil division

    std::vector<std::function<void()>> jobs;
    jobs.reserve(numBatches);

    for (size_t b = 0; b < numBatches; ++b) {
        size_t start = b * BATCH_SIZE;
        size_t end = std::min(start + BATCH_SIZE, totalEntities);

        jobs.emplace_back([&registry = this->registry, &entities, &recipeDb = this->recipeDb, start, end, dt]() {
            for (size_t i = start; i < end; ++i) {
                auto& machine = registry.get<MachineInstance>(entities[i]);
                const Recipe& recipe = recipeDb.getRecipe(machine.recipeId);
                processMachineInstance(machine, recipe, dt);
            }
        });
    }

    runPhaseAndWait(this->pool, std::move(jobs));
}
