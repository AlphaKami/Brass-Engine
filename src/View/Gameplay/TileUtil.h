#ifndef IRONBRASS_TILEUTIL_H
#define IRONBRASS_TILEUTIL_H

#include "../../Model/Map/TileCoord.h"
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <array>

struct TileAppearance {
    sf::IntRect textureRect; // atlas space
    sf::Vector2f origin = {0.f, 0.f}; // sprite anchor point

    TileAppearance() = default;
    TileAppearance(const sf::IntRect rect, const sf::Vector2f o)
        : textureRect(rect), origin(o) {}
};

namespace TileVertexBuilder {
    inline std::array<sf::Vertex, 6> buildSprite(const sf::Vector2f screenPos, const TileAppearance& appearance, const sf::Color color = sf::Color::White) {
        const float left   = screenPos.x - appearance.origin.x;
        const float top    = screenPos.y - appearance.origin.y;
        const float right  = left + static_cast<float>(appearance.textureRect.size.x);
        const float bottom = top  + static_cast<float>(appearance.textureRect.size.y);

        const auto u0 = static_cast<float>(appearance.textureRect.position.x);
        const auto v0 = static_cast<float>(appearance.textureRect.position.y);
        const auto u1 = u0 + static_cast<float>(appearance.textureRect.size.x);
        const auto v1 = v0 + static_cast<float>(appearance.textureRect.size.y);

        sf::Vertex topLeft;
        topLeft.position  = sf::Vector2f(left, top);
        topLeft.texCoords = sf::Vector2f(u0, v0);
        topLeft.color = color;

        sf::Vertex topRight;
        topRight.position  = sf::Vector2f(right, top);
        topRight.texCoords = sf::Vector2f(u1, v0);
        topRight.color = color;

        sf::Vertex bottomRight;
        bottomRight.position  = sf::Vector2f(right, bottom);
        bottomRight.texCoords = sf::Vector2f(u1, v1);
        bottomRight.color = color;

        sf::Vertex bottomLeft;
        bottomLeft.position  = sf::Vector2f(left, bottom);
        bottomLeft.texCoords = sf::Vector2f(u0, v1);
        bottomLeft.color = color;

        // triangles
        return {topLeft, topRight, bottomRight, topLeft, bottomRight, bottomLeft};
    }

    inline void appendSprite(sf::VertexArray& target, const sf::Vector2f screenPos, const TileAppearance& appearance, const sf::Color color = sf::Color::White) {
        for (const sf::Vertex& vertex : buildSprite(screenPos, appearance, color)) target.append(vertex);
    }
}

inline int drawOrderKey(const TileCoord& c) {
    return (c.x + c.y) * 4096 + c.z;
}

struct CompareByDrawOrder {
    bool operator()(const TileCoord& a, const TileCoord& b) const {
        return drawOrderKey(a) < drawOrderKey(b);
    }
};

#endif //IRONBRASS_TILEUTIL_H
