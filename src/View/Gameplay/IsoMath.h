#ifndef IRONBRASS_ISOMATH_H
#define IRONBRASS_ISOMATH_H

#include "../../Model/Map/TileCoord.h"
#include "SFML/System/Vector2.hpp"

/**
 * Size of an isometric tile.
 */
struct IsoMetrics { float tileWidth  = 32.f, tileHeight = 16.f, levelHeight = 16.f; };

namespace iso {
    /**
     * Returns the screen position of a tile.
     * @param t Coordinates of the tile
     * @param m Size of an isometric tile
     * @return Corresponding screen coordinates
     */
    inline sf::Vector2f toScreen(const TileCoord& t, const IsoMetrics& m) {
        return {
            static_cast<float>(t.x - t.y) * (m.tileWidth * 0.5f),
            static_cast<float>(t.x + t.y) * (m.tileHeight * 0.5f) - static_cast<float>(t.z) * m.levelHeight
        };
    }

    /**
     * Returns the tile based on a screen position.
     * Useful for mouse picking.
     * Does not consider z axes at the moment.
     * @param screen the position in screen coords.
     * @param m Size of am isometric tile
     * @param z The level being picked on.
     * @return  coords of the corresponding tile.
     */
    inline sf::Vector2f screenToGrid(sf::Vector2f screen, const IsoMetrics& m, const int z = 0) {
        const float hh = m.tileHeight * 0.5f;
        screen.y -= hh;
        screen.y += static_cast<float>(z) * m.levelHeight;
        const float hw = m.tileWidth * 0.5f;
        return {
            (screen.x / hw + screen.y / hh) * 0.5f,
            (screen.y / hh - screen.x / hw) * 0.5f
        };
    }

}

#endif //IRONBRASS_ISOMATH_H
