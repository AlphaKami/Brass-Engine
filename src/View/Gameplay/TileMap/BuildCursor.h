#ifndef IRONBRASS_BUILDCURSOR_H
#define IRONBRASS_BUILDCURSOR_H

#include "../../../Model/Map/TileCoord.h"
#include "../../../Model/Database/Buildable.h"
#include <SFML/System/Vector2.hpp>
#include <cstdint>

/** Where the player would build. Produced by the build controller, drawn by the renderer. */
struct BuildCursor {
    TileCoord target;
    BuildableId id;
    uint8_t orientation;
    bool valid;
    sf::Vector2f mouseWorld;
};

#endif //IRONBRASS_BUILDCURSOR_H
