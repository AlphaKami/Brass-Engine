#ifndef IRONBRASS_WORLDMAP_H
#define IRONBRASS_WORLDMAP_H

#include <unordered_map>
#include <memory>
#include "../Chunk.h"

template <typename TTile>
class WorldMap {
public:
    void setTile(const TileCoord& world, TTile tile);
    const TTile* getTile(const TileCoord& world) const;
    Chunk<TTile>* getChunk(ChunkCoord ck);

    void ensureLoaded(ChunkCoord ck);
    void unload(ChunkCoord ck);
private:
    std::unordered_map<ChunkCoord, std::unique_ptr<Chunk<TTile>>, ChunkCoordHash> chunks;

    std::pair<ChunkCoord, TileCoord> toChunkLocal(const TileCoord& w) const;
    Chunk<TTile>& getOrCreateChunk(ChunkCoord ck);
};

#endif //IRONBRASS_WORLDMAP_H
