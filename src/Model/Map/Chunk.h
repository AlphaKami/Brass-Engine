#ifndef IRONBRASS_CHUNK_H
#define IRONBRASS_CHUNK_H

#include "TileGrid.h"

template <typename TTile>
struct Chunk {
    static constexpr int SIZE = 16;

    TileGrid<TTile> tiles;
    bool meshDirty = true;
};

struct ChunkCoord {
    int32_t cx = 0, cy = 0;
    bool operator==(const ChunkCoord&) const = default;
};

struct ChunkCoordHash {
    std::size_t operator()(const ChunkCoord& c) const noexcept {
        return std::hash<int64_t>{}((static_cast<int64_t>(c.cx) << 32) ^ static_cast<uint32_t>(c.cy));
    }
};

#endif //IRONBRASS_CHUNK_H
