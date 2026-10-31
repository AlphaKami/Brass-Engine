#include "BuildableVisualLoader.h"

#include <fstream>
#include <stdexcept>
#include <nlohmann/json.hpp>
#include "../../../Core/JsonUtil.h"
#include "../../../Core/ResourceManager.h"

namespace {
    using json = nlohmann::json;
    using jsonutil::readArray;
    using jsonutil::requireKnownKeys;

    sf::Vector2f readOrigin(const json& obj) {
        if (!obj.contains("origin")) return {0.f, 0.f};
        const auto origin = readArray<2, float>(obj.at("origin"), "'origin'");
        return {origin[0], origin[1]};
    }

    // `item` is one entry of "orientations"; the index has already been read by the caller.
    OrientationVisual parseVisual(const json& item, const ResourceManager& resources) {
        const bool hasSprite = item.contains("sprite");
        const bool hasAnimation = item.contains("animation");
        if (hasSprite && hasAnimation) throw std::runtime_error("an orientation cannot have both 'sprite' and 'animation'");
        if (!hasSprite && !hasAnimation && item.contains("origin"))
            throw std::runtime_error("'origin' needs a 'sprite' or an 'animation'");
        const sf::Vector2f origin = readOrigin(item);

        OrientationVisual visual;
        try {
            if (hasSprite) {
                const json& sprite = item.at("sprite");
                requireKnownKeys(sprite, {"sheet", "tile"}, "'sprite'");
                const auto sheet = sprite.at("sheet").get<std::string>();
                const auto tile = sprite.at("tile").get<std::string>();
                visual.appearance = TileAppearance(resources.getTextureRect(sheet, tile), origin);
                visual.texture = &resources.getTextureSheet(sheet);
            } else if (hasAnimation) {
                const json& animation = item.at("animation");
                requireKnownKeys(animation, {"sheet", "name"}, "'animation'");
                const auto sheet = animation.at("sheet").get<std::string>();
                const auto name = animation.at("name").get<std::string>();
                visual.animation = &resources.getAnimation(sheet, name);
                visual.texture = &resources.getAnimatedTextureSheet(sheet);
                visual.appearance = TileAppearance(sf::IntRect{}, origin); // only the anchor matters
            }
        } catch (const std::out_of_range&) {
            throw std::runtime_error("unknown sheet, tile or animation referenced by '" + std::string(hasSprite ? "sprite" : "animation") + "'");
        }
        return visual;
    }
}

void BuildableVisualLoader::loadFromFile(const std::string& path, BuildableVisuals& out) const {
    std::ifstream file(path);
    if (!file.is_open()) throw std::runtime_error("[BuildableVisualLoader] Cannot open '" + path + "'");

    json data;
    try {
        file >> data;
    } catch (const json::parse_error& e) {
        throw std::runtime_error("[BuildableVisualLoader] JSON parsing error in '" + path + "': " + e.what());
    }

    try {
        requireKnownKeys(data, {"visuals"}, "file root");
    } catch (const std::exception& e) {
        throw std::runtime_error("[BuildableVisualLoader] '" + path + "': " + e.what());
    }

    // seen[id][orientation]: which orientations of the catalog got a visual
    std::vector<std::vector<bool>> seen(catalog.buildableCount());
    for (std::size_t id = 0; id < seen.size(); ++id)
        seen[id].assign(catalog.get(static_cast<BuildableId>(id)).orientations.size(), false);

    if (data.contains("visuals")) {
        for (const auto& entry : data.at("visuals")) {
            std::string name = "<invalid>";
            try {
                requireKnownKeys(entry, {"buildable", "orientations"}, "visual entry");
                name = entry.at("buildable").get<std::string>();
                const BuildableId id = catalog.getIdByName(name);

                for (const auto& item : entry.at("orientations")) {
                    int64_t index = -1;
                    try {
                        requireKnownKeys(item, {"orientation", "sprite", "animation", "origin"}, "orientation entry");
                        index = item.at("orientation").get<int64_t>();
                        if (index < 0 || static_cast<std::size_t>(index) >= seen[id].size())
                            throw std::runtime_error("the buildable has " + std::to_string(seen[id].size()) + " orientation(s)");
                        if (seen[id][index]) throw std::runtime_error("defined more than once");

                        out.set(id, static_cast<std::size_t>(index), parseVisual(item, resources));
                        seen[id][index] = true;
                    } catch (const std::exception& e) {
                        if (index < 0) throw;
                        throw std::runtime_error("orientation " + std::to_string(index) + ": " + e.what());
                    }
                }
            } catch (const std::exception& e) {
                throw std::runtime_error("[BuildableVisualLoader] '" + path + "' buildable '" + name + "': " + e.what());
            }
        }
    }

    // A missing visual would only show up later as an exception while drawing
    for (std::size_t id = 0; id < seen.size(); ++id) {
        for (std::size_t orientation = 0; orientation < seen[id].size(); ++orientation) {
            if (!seen[id][orientation])
                throw std::runtime_error("[BuildableVisualLoader] '" + path + "': no visual for buildable '"
                                         + catalog.get(static_cast<BuildableId>(id)).name + "' orientation " + std::to_string(orientation));
        }
    }
}