#ifndef IRONBRASS_HEIGHTGRIDVIEW_H
#define IRONBRASS_HEIGHTGRIDVIEW_H

#include "../IsoMath.h"
#include <SFML/Graphics.hpp>

class HeightGridView {
public:
    explicit HeightGridView(IsoMetrics metrics, float radiusPixels = 80.f);

    void update(const TileCoord& centerTile, const sf::Vector2f& mouseWorldPos);
    void hide();
    void draw(sf::RenderTarget& target, sf::RenderStates states = sf::RenderStates::Default) const;

private:
    IsoMetrics metrics;
    float radiusPixels;
    sf::VertexArray vertices;
};


#endif //IRONBRASS_HEIGHTGRIDVIEW_H
