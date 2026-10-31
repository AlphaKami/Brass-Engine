#ifndef IRONBRASS_TILEGRID_H
#define IRONBRASS_TILEGRID_H

#include "TileCoord.h"

template <typename TTile>
class TileGrid {
public:
    void set(const TileCoord& c, TTile tile) {
        m_tiles[c] = std::move(tile);
        m_dirty = true;
    }
    void remove(const TileCoord& c) {
        m_tiles.erase(c);
        m_dirty = true;
    }
    const TTile* get(const TileCoord& c) const {
        auto it = m_tiles.find(c);
        return it != m_tiles.end() ? &it->second : nullptr;
    }
    bool consumeDirty() {
        const bool wasDirty = m_dirty;
        m_dirty = false;
        return wasDirty;
    }

    auto begin() const { return m_tiles.begin(); }
    auto end() const { return m_tiles.end(); }

    typedef typename std::unordered_map<TileCoord, TTile, TileCoordHash>::const_iterator const_iterator;

private:
    std::unordered_map<TileCoord, TTile, TileCoordHash> m_tiles;
    bool m_dirty = true;
};

#endif //IRONBRASS_TILEGRID_H
