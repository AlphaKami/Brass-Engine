#ifndef IRONBRASS_PLACEMENTCONTEXT_H
#define IRONBRASS_PLACEMENTCONTEXT_H


#include <entt/entt.hpp>
#include "CellContents.h"

struct PlacementContext {
    const OccupancyGrid& occupancy;
    const entt::registry& registry;
    uint8_t orientationIndex;
};

#endif //IRONBRASS_PLACEMENTCONTEXT_H
