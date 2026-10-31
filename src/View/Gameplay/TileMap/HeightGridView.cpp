#include "HeightGridView.h"

HeightGridView::HeightGridView(const IsoMetrics metrics, const float radiusPixels)
    : metrics(metrics), radiusPixels(radiusPixels)
{
    this->vertices.setPrimitiveType(sf::PrimitiveType::Lines);
}

void HeightGridView::update(const TileCoord &centerTile, const sf::Vector2f &mouseWorldPos) {
    this->vertices.clear();

    const float hw = this->metrics.tileWidth * 0.5f;
    const float hh = this->metrics.tileHeight * 0.5f;
    const int tileRadius = static_cast<int>(std::ceil(this->radiusPixels / std::min(this->metrics.tileWidth, this->metrics.tileHeight))) + 1;

    for (int dx = -tileRadius; dx <= tileRadius; ++dx) {
        for (int dy = -tileRadius; dy <= tileRadius; ++dy) {
            const TileCoord coord{centerTile.x + dx, centerTile.y + dy, centerTile.z};
            const sf::Vector2f top = iso::toScreen(coord, this->metrics);
            const sf::Vector2f diamondCenter(top.x, top.y + hh);

            const float dist = std::hypot(diamondCenter.x - mouseWorldPos.x, diamondCenter.y - mouseWorldPos.y);
            if (dist > this->radiusPixels) continue;

            const float t = 1.f - dist / this->radiusPixels;
            const auto alpha = static_cast<std::uint8_t>(std::clamp(t, 0.f, 1.f) * 255.f);
            if (alpha == 0) continue;

            const sf::Color color(255, 255, 255, alpha);
            const sf::Vector2f right(top.x + hw, top.y + hh);
            const sf::Vector2f bottom(top.x, top.y + hh * 2.f);
            const sf::Vector2f left(top.x - hw, top.y + hh);

            const sf::Vertex edges[8] = {
                {top, color},    {right, color},
                {right, color},  {bottom, color},
                {bottom, color}, {left, color},
                {left, color},   {top, color}
            };
            for (const auto& v : edges) this->vertices.append(v);
        }
    }
}

void HeightGridView::hide() {
    this->vertices.clear();
}

void HeightGridView::draw(sf::RenderTarget &target, sf::RenderStates states) const {
    if (this->vertices.getVertexCount() == 0) return;
    states.texture = nullptr;
    target.draw(this->vertices, states);
}
