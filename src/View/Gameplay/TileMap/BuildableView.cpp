#include "BuildableView.h"
#include "../../ECS/Components/AnimationComponent.h"
#include <algorithm>

BuildableView::BuildableView(entt::registry& registry, const BuildableVisuals& visuals, IsoMetrics metrics)
    : registry(registry), visuals(visuals), isoMetrics(metrics)
{
}

void BuildableView::packTextures() {
    std::vector<const sf::Texture*> textures;
    for (std::size_t id = 0; id < this->visuals.buildableCount(); ++id)
        for (const OrientationVisual& visual : this->visuals.orientations(static_cast<BuildableId>(id)))
            textures.push_back(visual.texture);
    this->atlas.build(textures);
    this->builtVersion.reset();
}

void BuildableView::sync(const std::vector<TileSource>& placedTiles, const std::uint64_t structureVersion, const GhostEntry* ghost, const bool animationsChanged) {
    if (!this->builtVersion || *this->builtVersion != structureVersion || !ghostIsAsBuilt(ghost)) {
        this->rebuild(placedTiles, ghost);
        this->builtVersion = structureVersion;
        return;
    }

    if (ghost != nullptr && this->ghostSprite) {
        Sprite& sprite = this->sprites[*this->ghostSprite];
        bool changed = false;

        if (sprite.color != ghost->tint) {
            sprite.color = ghost->tint;
            changed = true;
        }
        if (ghost->frame && *ghost->frame != sprite.rect) {
            sprite.rect = *ghost->frame;
            changed = true;
        }
        if (changed) this->writeSprite(*this->ghostSprite);
    }

    if (animationsChanged) this->refreshAnimations();
}

void BuildableView::rebuild(const std::vector<TileSource>& placedTiles, const GhostEntry* ghost) {
    struct Entry {
        TileCoord coord;
        Sprite sprite;
        const sf::Texture* page;
        bool animated;
        bool isGhost;
    };
    std::vector<Entry> entries;

    auto add = [&](const TileCoord& coord, const OrientationVisual& visual, const sf::IntRect& rect, const sf::Color color, const entt::entity entity, const bool animated, const bool isGhost) {
        const TextureAtlas::Slot slot = this->atlas.find(visual.texture);
        entries.push_back({coord, Sprite{entity, iso::toScreen(coord, this->isoMetrics), visual.appearance.origin, rect, slot.offset, color}, slot.page, animated, isGhost});
    };

    const auto view = this->registry.view<const TileCoord, const Buildable>();
    for (auto [entity, pos, built] : view.each()) {
        const OrientationVisual& visual = this->visuals.get(built.id, built.orientation);
        sf::IntRect rect = visual.appearance.textureRect;
        bool animated = false;
        if (const auto* anim = this->registry.try_get<AnimationComponent>(entity)) {
            animated = true;
            if (const auto frame = anim->player.currentRect()) rect = *frame;
        }
        add(pos, visual, rect, sf::Color::White, entity, animated, false);
    }

    for (const TileSource& tile : placedTiles) {
        const OrientationVisual& visual = this->visuals.get(tile.id, tile.orientation);
        add(tile.origin, visual, visual.appearance.textureRect, sf::Color::White, entt::null, false, false);
    }

    if (ghost != nullptr) {
        const OrientationVisual& visual = this->visuals.get(ghost->id, ghost->orientation);
        sf::IntRect rect = visual.animation != nullptr ? visual.animation->frames.front() : visual.appearance.textureRect;
        if (ghost->frame) rect = *ghost->frame;
        add(ghost->origin, visual, rect, ghost->tint, entt::null, false, true);
    }

    std::stable_sort(entries.begin(), entries.end(), [](const Entry& a, const Entry& b) {
        return drawOrderKey(a.coord) < drawOrderKey(b.coord);
    });

    this->vertices.assign(entries.size() * 6, sf::Vertex{});
    this->runs.clear();
    this->sprites.clear();
    this->sprites.reserve(entries.size());
    this->animatedSprites.clear();
    this->ghostSprite.reset();

    for (std::size_t i = 0; i < entries.size(); ++i) {
        const Entry& entry = entries[i];

        if (this->runs.empty() || this->runs.back().texture != entry.page) {
            this->runs.push_back({entry.page, i * 6, 0});
        }
        this->runs.back().vertexCount += 6;

        this->sprites.push_back(entry.sprite);
        this->writeSprite(i);

        if (entry.animated) this->animatedSprites.push_back(i);
        if (entry.isGhost) this->ghostSprite = i;
    }

    if (ghost != nullptr) this->builtGhost = GhostIdentity{ghost->origin, ghost->id, ghost->orientation};
    else this->builtGhost.reset();
    ++this->rebuilds;
}

void BuildableView::writeSprite(const std::size_t index) {
    const Sprite& sprite = this->sprites[index];
    const sf::IntRect atlasRect(sprite.rect.position + sprite.atlasOffset, sprite.rect.size);
    const auto quad = TileVertexBuilder::buildSprite(sprite.screenPos, TileAppearance(atlasRect, sprite.origin), sprite.color);
    std::ranges::copy(quad, this->vertices.begin() + static_cast<std::ptrdiff_t>(index * 6));
}

void BuildableView::refreshAnimations() {
    for (const std::size_t index : this->animatedSprites) {
        Sprite& sprite = this->sprites[index];
        if (!this->registry.valid(sprite.entity)) continue;

        const auto* anim = this->registry.try_get<AnimationComponent>(sprite.entity);
        if (anim == nullptr) continue;

        const auto frame = anim->player.currentRect();
        if (!frame || *frame == sprite.rect) continue;

        sprite.rect = *frame;
        this->writeSprite(index);
    }
}

bool BuildableView::ghostIsAsBuilt(const GhostEntry* ghost) const {
    if (ghost == nullptr) return !this->builtGhost.has_value();
    if (!this->builtGhost) return false;

    const GhostIdentity& built = *this->builtGhost;
    return built.origin.x == ghost->origin.x && built.origin.y == ghost->origin.y && built.origin.z == ghost->origin.z
        && built.id == ghost->id && built.orientation == ghost->orientation;
}

void BuildableView::draw(sf::RenderTarget &target, sf::RenderStates states) const {
    for (const auto& run : this->runs) {
        states.texture = run.texture;
        target.draw(&this->vertices[run.vertexStart], run.vertexCount, sf::PrimitiveType::Triangles, states);
    }
}
