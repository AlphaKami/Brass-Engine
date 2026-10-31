#include "PlacementService.h"
#include "Predicates/CellPredicates.h"

PlacementService::PlacementService(const WorldBounds& bounds, Simulation& simulation)
    : bounds(bounds), wallMarker(simulation.getMarkerRegistry().resolve("wall")), simulation(simulation) {
    noWallAbove = std::make_shared<AtOffset>(0, 0, 1, std::make_shared<Not>(std::make_shared<HasMarker>(wallMarker)));
}

bool PlacementService::canPlace(const BuildableOrientation& orientation, const uint8_t orientationIndex, const TileCoord& origin) const {
    const PlacementContext ctx{this->simulation.getOccupancyGrid(), this->simulation.getRegistry(), orientationIndex};

    for (const auto& cell : orientation.cells) {
        const TileCoord coord = absoluteCell(origin, cell);

        if (occupiesCell(cell)) {
            if (!this->bounds.inBounds(coord)) return false;
            if (!noWallAbove->isSatisfied(ctx, coord)) return false;
        }

        if (!cellSatisfied(ctx, cell, coord)) return false;
    }
    return true;
}

bool PlacementService::cellSatisfied(const PlacementContext& ctx, const BuildableCell& cell, const TileCoord& coord) {
    if (cell.predicate != nullptr) return cell.predicate->isSatisfied(ctx, coord);
    if (occupiesCell(cell)) {
        static const CellPredicatePtr defaultEmpty = std::make_shared<EmptyCell>();
        return defaultEmpty->isSatisfied(ctx, coord);
    }
    return true;
}

bool PlacementService::tryPlace(const BuildableId id, const uint8_t orientationIndex, const TileCoord& origin) const {
    const BuildableDef& def = this->simulation.getBuildableCatalog().get(id);
    const BuildableOrientation& orientation = def.orientations.at(orientationIndex);

    if (!canPlace(orientation, orientationIndex, origin)) return false;

    if (def.isEntity) {
        this->simulation.spawnBuildable(id, orientationIndex, origin);
    } else {
        this->simulation.placeTile(id, orientationIndex, origin);
    }
    return true;
}

void PlacementService::remove(const entt::entity entity) const {
    this->simulation.removeBuildable(entity);
}