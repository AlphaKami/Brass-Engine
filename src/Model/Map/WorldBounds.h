#ifndef IRONBRASS_WORLDBOUNDS_H
#define IRONBRASS_WORLDBOUNDS_H


#include "TileCoord.h"

/** Extent of the buildable world. */
struct WorldBounds { // Note: It's Temporary, it will be replaced by the chunk logic.
    int width;
    int depth;
    int levels;

    [[nodiscard]] bool inBounds(const TileCoord& c) const {
        return c.x >= 0 && c.x < width && c.y >= 0 && c.y < depth && c.z >= 0 && c.z < levels;
    }
};

#endif //IRONBRASS_WORLDBOUNDS_H
