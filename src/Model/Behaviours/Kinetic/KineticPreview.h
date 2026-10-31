#ifndef IRONBRASS_KINETICPREVIEW_H
#define IRONBRASS_KINETICPREVIEW_H

#include <entt/entt.hpp>
#include "../../BuildSystem/CellContents.h"
#include "../../Database/Buildable.h"
#include "../../Database/KineticDatabase.h"

/**
 * How a kinetic buildable would turn if it were placed somewhere, computed without placing it (the registry and the
 * occupancy grid are only read). Meant for the placement ghost, so that it looks like a placed one: aligned with
 * its neighbors, and turning with them in the same direction and at the same speed.
 */
struct KineticPreview {
    const KineticDef* def = nullptr;

    /** The existing node that would drive it. Null if it would not touch any: it would just sit still. */
    entt::entity parent = entt::null;
    double ratio = 1.0;   // same meaning as in KineticDrive
    double offset = 0.0;

    /** Angle in turns that the previewed buildable would have right now. */
    [[nodiscard]] double angle(const entt::registry& registry) const;
};

/**
 * Finds the node that would drive `def` placed at `origin`, as resolveKineticNetworks would do.
 *
 * Links are looked for from both sides, because not all of them are declared by both nodes: the ones that `def`
 * declares, and the ones that its neighbors declare towards the cells (and markers) that the buildable would occupy.
 * If it would touch several nodes, the oldest one is taken, which is the one that would impose its phase.
 * The cost doesn't depend on how much is placed, but it is not free: don't call it every frame
 * for the same placement.
 */
[[nodiscard]] KineticPreview findKineticPreview(const entt::registry& registry, const OccupancyGrid& occupancy, const KineticDatabase& database, const KineticDef& def, const BuildableOrientation& orientation, const TileCoord& origin);

#endif //IRONBRASS_KINETICPREVIEW_H
