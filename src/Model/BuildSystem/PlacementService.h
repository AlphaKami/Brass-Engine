#ifndef IRONBRASS_PLACEMENTSERVICE_H
#define IRONBRASS_PLACEMENTSERVICE_H


#include "../Simulation.h"
#include "../Map/WorldBounds.h"
#include "../Database/Buildable.h"
#include "Predicates/CellPredicates.h"

class PlacementService {
public:
    PlacementService(const WorldBounds& bounds, Simulation& simulation);

    [[nodiscard]] bool canPlace(const BuildableOrientation& orientation, uint8_t orientationIndex, const TileCoord& origin) const;
    [[nodiscard]] bool tryPlace(BuildableId id, uint8_t orientationIndex, const TileCoord& origin) const;
    void remove(entt::entity entity) const;

private:
    const WorldBounds& bounds;
    MarkerId wallMarker;
    Simulation& simulation;

    CellPredicatePtr noWallAbove;

    /** A cell that deposits at least one marker occupies space. Cells with no markers only carry a predicate. */
    [[nodiscard]] static bool occupiesCell(const BuildableCell& cell) { return !cell.markers.empty(); }
    [[nodiscard]] static bool cellSatisfied(const PlacementContext& ctx, const BuildableCell& cell, const TileCoord& coord);
};


#endif //IRONBRASS_PLACEMENTSERVICE_H