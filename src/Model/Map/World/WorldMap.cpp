#include "WorldMap.h"

template<typename TTile>
void WorldMap<TTile>::setTile(const TileCoord &world, TTile tile) {
    auto [ck, local] = toChunkLocal(world);
    getOrCreateChunk(ck).tiles.set(local, std::move(tile));
}

template<typename TTile>
const TTile * WorldMap<TTile>::getTile(const TileCoord &world) const {
    auto [ck, local] = toChunkLocal(world);
    auto it = this->chunks.find(ck);
    if (it == this->chunks.end()) return nullptr;
    return it->second->tiles.get(local);
}

template<typename TTile>
Chunk<TTile> * WorldMap<TTile>::getChunk(ChunkCoord ck) {
    auto it = this->chunks.find(ck);
    return it != this->chunks.end() ? it->second.get() : nullptr;
}

template<typename TTile>
void WorldMap<TTile>::ensureLoaded(ChunkCoord ck) {
    if (!this->chunks.contains(ck)) {
        getOrCreateChunk(ck); // TODO: link here the procedural generator
    }
}

template<typename TTile>
void WorldMap<TTile>::unload(ChunkCoord ck) {
    this->chunks.erase(ck);
}

template<typename TTile>
std::pair<ChunkCoord, TileCoord> WorldMap<TTile>::toChunkLocal(const TileCoord &w) const {
    constexpr int size = Chunk<TTile>::SIZE;
    const int32_t cx = w.x >= 0 ? w.x / size : (w.x - size + 1) / size;
    const int32_t cy = w.y >= 0 ? w.y / size : (w.y - size + 1) / size;
    TileCoord local{ w.x - cx * size, w.y - cy * size, w.z };
    return { {cx, cy}, local };
}

template<typename TTile>
Chunk<TTile> & WorldMap<TTile>::getOrCreateChunk(ChunkCoord ck) {
    auto& ptr = this->chunks[ck];
    if (!ptr) ptr = std::make_unique<Chunk<TTile>>();
    return *ptr;
}