#include "TextureAtlas.h"

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <SFML/Graphics/Image.hpp>
#include "AtlasPacker.h"

namespace {
    constexpr unsigned PADDING = 1; // transparent pixels between sheets: nothing can bleed into a neighbor
}

void TextureAtlas::build(const std::vector<const sf::Texture*>& sources) {
    this->pages.clear();
    this->slots.clear();

    std::vector<const sf::Texture*> unique;
    for (const sf::Texture* texture : sources) {
        if (texture != nullptr && std::ranges::find(unique, texture) == unique.end()) unique.push_back(texture);
    }
    if (unique.empty()) return;

    std::vector<sf::Vector2u> sizes;
    sizes.reserve(unique.size());
    for (const sf::Texture* texture : unique) sizes.push_back(texture->getSize());

    const AtlasPacker::Result packed = AtlasPacker::pack(sizes, sf::Texture::getMaximumSize(), PADDING);

    std::vector<sf::Image> images;
    images.reserve(packed.pageSizes.size());
    for (const sf::Vector2u& size : packed.pageSizes) images.emplace_back(size, sf::Color::Transparent);

    for (std::size_t i = 0; i < unique.size(); ++i) {
        const AtlasPacker::Placement& placement = packed.placements[i];
        if (!placement.packed) continue;
        if (!images[placement.page].copy(unique[i]->copyToImage(), placement.position)) {
            throw std::runtime_error("[TextureAtlas] cannot copy a texture into the atlas");
        }
    }

    for (const auto & image : images) {
        auto texture = std::make_unique<sf::Texture>();
        if (!texture->loadFromImage(image))
            throw std::runtime_error("[TextureAtlas] cannot create the atlas texture");
        texture->setSmooth(unique.front()->isSmooth());
        this->pages.push_back(std::move(texture));
    }

    for (std::size_t i = 0; i < unique.size(); ++i) {
        const AtlasPacker::Placement& placement = packed.placements[i];
        if (!placement.packed) {
            this->slots[unique[i]] = Slot{unique[i], {0, 0}};
        }
        else {
            this->slots[unique[i]] = Slot{this->pages[placement.page].get(), sf::Vector2i(placement.position)};
        }
    }

    std::cout << "[Atlas] " << unique.size() << " texture(s) packed into " << this->pages.size() << " page(s)";
    for (const sf::Vector2u& size : packed.pageSizes) std::cout << " " << size.x << "x" << size.y;
    std::cout << "\n";
}