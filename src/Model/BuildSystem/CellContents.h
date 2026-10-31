#ifndef IRONBRASS_CELLCONTENTS_H
#define IRONBRASS_CELLCONTENTS_H

#include <entt/entt.hpp>
#include <vector>
#include "MarkerId.h"
#include "../Map/TileCoord.h"
#include "../Map/TileGrid.h"

using BuildableId = std::uint32_t;

/**
 * Describes a placed static tile; comes with all needed to be found/removed as
 * there isn't a entity on which rely.
 * Does not live in the entity registry
 */
struct TileSource {
    BuildableId id;
    uint8_t orientation;
    TileCoord origin;
    bool operator==(const TileSource&) const = default;
};

/** Who deposited a certain marker or claim on a cell. */
using CellSource = std::variant<entt::entity, TileSource>;

/** A marker deposited on a cell and his source to backtrack */
struct CellMarker {
    MarkerId marker;
    CellSource source;
};

/**
 * Everything existing on a single cell of the grid
 * `markers` lists every single deposited marker
 * `isEmpty()` returns true if there are no markers in the cell
 */
struct CellContents {;
    std::vector<CellMarker> markers;

    [[nodiscard]] bool isEmpty() const { return markers.empty(); }
};

using OccupancyGrid = TileGrid<CellContents>;

/** Reads the orientation of a source, whether it is an entity or a static tile.
 *  Templated on TBuildableComponent solely to avoid including Buildable.h here.
 *  The caller must explicitly instantiate it with <Buildable>.
 *
 */
template <typename TBuildableComponent>
uint8_t orientationOf(const CellSource& source, const entt::registry& registry) {
    if (const auto* entity = std::get_if<entt::entity>(&source)) {
        return registry.all_of<TBuildableComponent>(*entity) ? registry.get<TBuildableComponent>(*entity).orientation : 0;
    }
    return std::get<TileSource>(source).orientation;
}

#endif //IRONBRASS_CELLCONTENTS_H
