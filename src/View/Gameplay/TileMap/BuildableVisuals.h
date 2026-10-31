#ifndef IRONBRASS_BUILDABLEVISUALS_H
#define IRONBRASS_BUILDABLEVISUALS_H

#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>
#include <SFML/Graphics/Texture.hpp>
#include "../../../Core/Animation/Animation.h"
#include "../../../Model/Database/Buildable.h"
#include "../TileUtil.h"

/** How one orientation of a buildable looks like. Everything here is owned by the ResourceManager. */
struct OrientationVisual {
    TileAppearance appearance;
    const Animation* animation = nullptr;
    const sf::Texture* texture = nullptr;
};

/**
 * Graphic side of the BuildableCatalog.
 */
class BuildableVisuals {
public:
    void set(const BuildableId id, const std::size_t orientation, OrientationVisual visual) {
        if (visuals.size() <= id) visuals.resize(id + 1);
        if (visuals[id].size() <= orientation) visuals[id].resize(orientation + 1);
        visuals[id][orientation] = std::move(visual);
    }

    /** @throws std::out_of_range if the visuals of that buildable were never loaded */
    [[nodiscard]] const OrientationVisual& get(const BuildableId id, const std::uint8_t orientation) const {
        return visuals.at(id).at(orientation);
    }

    [[nodiscard]] std::size_t buildableCount() const { return visuals.size(); }
    [[nodiscard]] const std::vector<OrientationVisual>& orientations(const BuildableId id) const { return visuals.at(id); }

private:
    std::vector<std::vector<OrientationVisual>> visuals; // [buildable id][orientation]
};

#endif //IRONBRASS_BUILDABLEVISUALS_H
