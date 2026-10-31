#ifndef IRONBRASS_TILECOORD_H
#define IRONBRASS_TILECOORD_H

#include <cstdint>
#include <functional>

/**
 * Coordinates of a tile in 3D space.
 * Values are saved in int32_t
 */
struct TileCoord {
    int32_t x = 0, y = 0, z = 0;

    bool operator==(const TileCoord& other) const {
        return x == other.x && y == other.y && z == other.z;
    }
};


struct TileCoordHash {
    /**
    * Save the tile in a 64-bit hash.
    * x and y occupy 21 bit, z occupy 22 bit
    */
    std::size_t operator()(const TileCoord& c) const noexcept {
        const uint64_t hx = static_cast<uint32_t>(c.x);
        const uint64_t hy = static_cast<uint32_t>(c.y);
        const uint64_t hz = static_cast<uint32_t>(c.z);
        return std::hash<uint64_t>{}((hx << 42) ^ (hy << 21) ^ hz);
    }
};

#endif //IRONBRASS_TILECOORD_H
