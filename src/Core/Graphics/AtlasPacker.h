#ifndef IRONBRASS_ATLASPACKER_H
#define IRONBRASS_ATLASPACKER_H

#include <cstddef>
#include <vector>
#include <SFML/System/Vector2.hpp>

/**
 * Decides where to put a set of rectangles inside as few pages as possible.
 * It knows nothing about textures; TextureAtlas applies the result.
 */
namespace AtlasPacker {
    struct Placement {
        bool packed = false; // false if the rectangle is larger than a page
        std::size_t page = 0;
        sf::Vector2u position{0, 0}; // top-left corner in the page
    };

    struct Result {
        std::vector<Placement> placements; // same order as the input sizes
        std::vector<sf::Vector2u> pageSizes; // never exceed maxPageSize on either side
    };

    /**
     * @param sizes        size of each rectangle
     * @param maxPageSize  maximum width and height of a page
     * @param padding      empty pixels left after every rectangle so that neighbors can't bleed
     */
    [[nodiscard]] Result pack(const std::vector<sf::Vector2u>& sizes, unsigned maxPageSize, unsigned padding);
}


#endif //IRONBRASS_ATLASPACKER_H
