#include "FactoryRenderer.h"

#include "../Gameplay/TileMap/BuildableVisualLoader.h"
#include "../ECS/Systems/AnimationSystem.h"
#include "../ECS/Systems/AnimationAttachSystem.h"
#include "../ECS/Systems/KineticFrameSystem.h"
#include <cmath>

namespace {
    // The visuals must be loaded before the views that reference them are built.
    BuildableVisuals loadVisuals(Simulation& sim, const ResourceManager& resources) {
        BuildableVisuals visuals;
        BuildableVisualLoader(sim.getBuildableCatalog(), resources).loadFromFile("Resources/Data/BuildSystemData/buildables_visuals.json", visuals);
        return visuals;
    }
}

FactoryRenderer::FactoryRenderer(sf::RenderWindow& window, Simulation& sim, ResourceManager& resources, const IsoMetrics& metrics)
    : window(window), sim(sim), resources(resources), visuals(loadVisuals(sim, resources)),
    buildableView(sim.getRegistry(), this->visuals, metrics), heightGrid(metrics)
{
    const auto windowSize = window.getSize();
    this->tilemapRT = sf::RenderTexture(windowSize);
    this->tilemapRT.setSmooth(true);

    sf::Shader& upscalingShader = resources.getFragShader("UpscalingShader");
    const sf::Vector2u atlasSize = resources.getTextureSheet("TestTilesheet").getSize();
    upscalingShader.setUniform("textureSize", sf::Vector2f(
        static_cast<float>(atlasSize.x),
        static_cast<float>(atlasSize.y)
    ));
    this->buildableView.packTextures();
}

void FactoryRenderer::update(float dt, const std::optional<BuildCursor> &cursor) {
    entt::registry& registry = this->sim.getRegistry();
    attachAnimations(registry, this->visuals);
    bool animationsChanged = updateAnimations(registry, dt);
    animationsChanged |= updateKineticFrames(registry);

    std::optional<GhostEntry> ghost;
    if (cursor) {
        static constexpr sf::Color validTint{120, 255, 120, 160};
        static constexpr sf::Color invalidTint{255, 90, 90, 160};
        ghost = GhostEntry{cursor->target, cursor->id, cursor->orientation, cursor->valid ? validTint : invalidTint};
        ghost->frame = ghostFrame(*cursor, this->visuals.get(cursor->id, cursor->orientation));

        this->heightGrid.update(cursor->target, cursor->mouseWorld);
    } else {
        this->heightGrid.hide();
    }

    this->buildableView.sync(this->sim.getPlacedTiles(), this->sim.getBuildablesVersion(), ghost ? &*ghost : nullptr, animationsChanged);
}

void FactoryRenderer::render(const Camera &camera) {
    const sf::Vector2f center = camera.view().getCenter();
    const float zoomLevel = camera.zoom();
    const sf::Vector2f roundedCenter(std::round(center.x), std::round(center.y));
    const sf::Vector2f subpixelRemainder = center - roundedCenter;

    this->panOnlyView.setSize(camera.view().getSize() / zoomLevel);
    this->panOnlyView.setCenter(roundedCenter);

    this->tilemapRT.clear(sf::Color::Transparent);
    this->tilemapRT.setView(this->panOnlyView);
    this->heightGrid.draw(this->tilemapRT, sf::RenderStates::Default);
    this->buildableView.draw(this->tilemapRT, sf::RenderStates::Default);
    this->tilemapRT.display();

    this->window.setView(this->window.getDefaultView());

    sf::Sprite quad(this->tilemapRT.getTexture());
    quad.setOrigin(sf::Vector2f(this->tilemapRT.getSize()) * 0.5f);

    const sf::Vector2f screenCenter = sf::Vector2f(this->window.getSize()) * 0.5f;
    const sf::Vector2f screenOffset = -subpixelRemainder / zoomLevel;
    quad.setPosition(screenCenter + screenOffset);

    quad.setScale({1.f / zoomLevel, 1.f / zoomLevel});

    sf::Shader& pixelArtShader = this->resources.getFragShader("UpscalingShader");
    pixelArtShader.setUniform("texture", sf::Shader::CurrentTexture);
    pixelArtShader.setUniform("textureSize", sf::Vector2f(this->tilemapRT.getSize()));

    sf::RenderStates states;
    states.shader = &pixelArtShader;
    this->window.draw(quad, states);

    this->window.setView(this->window.getDefaultView());
}

std::optional<sf::IntRect> FactoryRenderer::ghostFrame(const BuildCursor &cursor, const OrientationVisual &visual) {
    const std::uint64_t version = this->sim.getBuildablesVersion();

    const bool stale = !this->ghostPreviewKey
          || this->ghostPreviewKey->version != version
          || this->ghostPreviewKey->id != cursor.id
          || this->ghostPreviewKey->orientation != cursor.orientation
          || this->ghostPreviewKey->origin.x != cursor.target.x
          || this->ghostPreviewKey->origin.y != cursor.target.y
          || this->ghostPreviewKey->origin.z != cursor.target.z;
    if (stale) {
        this->ghostPreview = this->sim.previewKinetic(cursor.id, cursor.orientation, cursor.target);
        this->ghostPreviewKey = GhostPreviewKey{version, cursor.target, cursor.id, cursor.orientation};
    }

    if (!this->ghostPreview || visual.animation == nullptr || visual.animation->frames.empty()) return std::nullopt;

    const Animation& animation = *visual.animation;
    const double angle = this->ghostPreview->angle(this->sim.getRegistry());
    return animation.frames[kineticFrameIndex(angle, this->ghostPreview->def->frameSpan, animation.frames.size())];
}
