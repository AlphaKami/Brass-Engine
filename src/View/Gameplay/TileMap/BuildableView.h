#ifndef IRONBRASS_BUILDABLEVIEW_H
#define IRONBRASS_BUILDABLEVIEW_H

#include <cstdint>
#include <optional>
#include <vector>
#include <entt/entt.hpp>
#include "BuildableVisuals.h"
#include "../../../Core/Graphics/TextureAtlas.h"
#include "../IsoMath.h"
#include "../TileUtil.h"
#include "../../../Model/Database/Buildable.h"
#include "../../../Model/BuildSystem/CellContents.h"
#include <SFML/Graphics.hpp>

struct GhostEntry {
    TileCoord origin;
    BuildableId id;
    uint8_t orientation;
    sf::Color tint;
    std::optional<sf::IntRect> frame;
};

/**
 * Draws every placed buildable (entities and tiles) and the placement ghost, sorted by depth.
 *
 * The expensive part, sorting everything and generating the vertices, only depends on what is placed where, so it is redone only when that changes.
 * An animated sprite changing frame only rewrites its own quad.
 * All the sprites are drawn from a TextureAtlas, so that the draw calls don't depend on how the sprites of different sheets interleave in depth order.
 */
class BuildableView {
public:
    BuildableView(entt::registry& registry, const BuildableVisuals& visuals, IsoMetrics metrics);

    /**
     * Packs the textures of every buildable in the catalog into an atlas (usually a single page, hence a single draw call).
     * Call it once, after the catalog is loaded and before the first sync/rebuild.
     * Without it every sprite is drawn with its own texture.
     * Needs an OpenGL context.
     */
    void packTextures();

    /**
     * Brings the view up-to-date doing the least work that is correct.
     * Meant to be called every frame.
     * @param placedTiles vector of TileSource
     * @param structureVersion changes whenever something is placed or removed
     * @param ghost the placement preview, or nullptr
     * @param animationsChanged whether some animation may have changed frame since the last call
     */
    void sync(const std::vector<TileSource>& placedTiles, std::uint64_t structureVersion, const GhostEntry* ghost, bool animationsChanged);

    /** Sorts and regenerates everything. sync does it by itself when needed. */
    void rebuild(const std::vector<TileSource>& placedTiles, const GhostEntry* ghost = nullptr);

    void draw(sf::RenderTarget& target, sf::RenderStates states = sf::RenderStates::Default) const;

    /** Draw calls issued by draw(): one per run of sprites sharing a texture page. */
    [[nodiscard]] std::size_t drawCallCount() const { return runs.size(); }
    /** How many times everything was regenerated. */
    [[nodiscard]] std::size_t rebuildCount() const { return rebuilds; }

private:
    struct Run {
        const sf::Texture* texture;
        std::size_t vertexStart;
        std::size_t vertexCount;
    };

    /** One drawn sprite. Sprite 'i' owns the vertices [6i, 6i + 6) in draw order. */
    struct Sprite {
        entt::entity entity = entt::null; // null for tiles and for the ghost
        sf::Vector2f screenPos;
        sf::Vector2f origin;
        sf::IntRect rect;
        sf::Vector2i atlasOffset; // where that sheet starts in the atlas page
        sf::Color color;
    };

    struct GhostIdentity {
        TileCoord origin;
        BuildableId id;
        uint8_t orientation;
    };

    void writeSprite(std::size_t index);
    void refreshAnimations();
    [[nodiscard]] bool ghostIsAsBuilt(const GhostEntry* ghost) const;

    entt::registry& registry;
    IsoMetrics isoMetrics;
    TextureAtlas atlas;

    const BuildableVisuals& visuals;

    std::vector<sf::Vertex> vertices;
    std::vector<Run> runs;
    std::vector<Sprite> sprites;
    std::vector<std::size_t> animatedSprites; // indices of the sprites that belong to an animated entity
    std::optional<std::size_t> ghostSprite;
    std::optional<GhostIdentity> builtGhost;

    std::optional<std::uint64_t> builtVersion;
    std::size_t rebuilds = 0;
};


#endif //IRONBRASS_BUILDABLEVIEW_H
