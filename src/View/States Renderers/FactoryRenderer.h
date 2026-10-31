#ifndef IRONBRASS_FACTORYRENDERER_H
#define IRONBRASS_FACTORYRENDERER_H

#include "../Gameplay/TileMap/BuildCursor.h"
#include "../Gameplay/Camera.h"
#include "../Gameplay/IsoMath.h"
#include "../Gameplay/TileMap/BuildableView.h"
#include "../Gameplay/TileMap/BuildableVisuals.h"
#include "../Gameplay/TileMap/HeightGridView.h"
#include "../../Core/ResourceManager.h"
#include "../../Model/Simulation.h"
#include "../../Model/Behaviours/Kinetic/KineticPreview.h"

#include <optional>

/**
 * Everything needed to draw the factory: visuals, tile views, ghost, offscreen pass and upscaling.
 * It only reads the model.
 */
class FactoryRenderer {
public:
    FactoryRenderer(sf::RenderWindow& window, Simulation& sim, ResourceManager& resources, const IsoMetrics& metrics);

    /** Advances animations and refreshes the meshes. `cursor` is empty when not building. */
    void update(float dt, const std::optional<BuildCursor>& cursor);
    void render(const Camera& camera);

private:
    sf::RenderWindow& window;
    Simulation& sim;
    ResourceManager& resources;

    BuildableVisuals visuals; // must be declared before buildableView
    BuildableView buildableView;
    HeightGridView heightGrid;

    sf::RenderTexture tilemapRT;
    sf::View panOnlyView;

    struct GhostPreviewKey {
        std::uint64_t version;
        TileCoord origin;
        BuildableId id;
        uint8_t orientation;
    };
    std::optional<GhostPreviewKey> ghostPreviewKey;
    std::optional<KineticPreview> ghostPreview;

    std::optional<sf::IntRect> ghostFrame(const BuildCursor& cursor, const OrientationVisual& visual);
};


#endif //IRONBRASS_FACTORYRENDERER_H
