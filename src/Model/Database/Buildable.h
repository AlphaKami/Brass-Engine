#ifndef IRONBRASS_BUILDABLE_H
#define IRONBRASS_BUILDABLE_H

#include "../../View/Gameplay/TileUtil.h"
#include "../../Core/Animation/Animation.h"
#include "../BuildSystem/Predicates/ICellPredicate.h"
#include "../BuildSystem/MarkerId.h"
#include <string>
#include <cstdint>
#include <vector>

using BuildableId = std::uint32_t;

struct BuildableCell {
    int32_t dx = 0, dy = 0, dz = 0;
    std::vector<MarkerId> markers;
    CellPredicatePtr predicate = nullptr;
};

struct BuildableOrientation {
    std::vector<BuildableCell> cells;
};

struct BuildableDef {
    BuildableId id{};
    std::string name;
    std::vector<BuildableOrientation> orientations; // @requires length >= 1

    bool isEntity = false;
    bool playerPlaceable = true;
};

/** Runtime marker: placed entity from this BuildableDef, with this orientation. */
struct Buildable {
    BuildableId id{};
    uint8_t orientation = 0;
};

/**
 * Compute the actual coords of a cell.
 * Cell position are relative to the center of the buildable.
 * @param origin coords the central point
 * @param cell target cell
 * @return absolute position of the cell
 */
inline TileCoord absoluteCell(const TileCoord& origin, const BuildableCell& cell) {
    return TileCoord{ origin.x + cell.dx, origin.y + cell.dy, origin.z + cell.dz };
}

#endif //IRONBRASS_BUILDABLE_H
