#ifndef IRONBRASS_TEXTUREATLAS_H
#define IRONBRASS_TEXTUREATLAS_H

#include <memory>
#include <unordered_map>
#include <vector>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>

/**
 * Copies several textures into one (or, if the GPU can't hold them in one, a few) so that sprites coming from different sheets can be drawn with a single draw call.
 * The original textures are left untouched.
 *
 * Sprites keep using rectangles in the space of their own sheet: find() tells where the sheet ended up, and the offset has to be added to the rectangle.
 */
class TextureAtlas {
public:
    struct Slot {
        const sf::Texture* page = nullptr;
        sf::Vector2i offset{0, 0};
    };

    /**
     * Packs the given textures.
     * Duplicates and nulls are ignored.
     * Needs an OpenGL context.
     * Textures that can't be packed (larger than the GPU limit) are not moved.
     */
    void build(const std::vector<const sf::Texture*>& sources);

    /** @return where `source` lives; a texture that was not packed is its own page, with no offset. */
    [[nodiscard]] Slot find(const sf::Texture* source) const {
        const auto it = slots.find(source);
        return it != slots.end() ? it->second : Slot{source, {0, 0}};
    }

    [[nodiscard]] std::size_t pageCount() const { return pages.size(); }

private:
    std::vector<std::unique_ptr<sf::Texture>> pages;
    std::unordered_map<const sf::Texture*, Slot> slots;
};


#endif //IRONBRASS_TEXTUREATLAS_H
