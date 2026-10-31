#include "BuildableLoader.h"

#include "BuildableLoader.h"

#include "../../Core/JsonUtil.h"
#include "Predicates/CellPredicates.h"
#include <algorithm>
#include <array>
#include <fstream>
#include <initializer_list>
#include <optional>
#include <stdexcept>
#include <string_view>

namespace {
    using json = nlohmann::json;

    using jsonutil::requireKnownKeys;
    using jsonutil::readArray;
}

BuildableLoader::BuildableLoader(BuildableCatalog& catalog, const MarkerRegistry& markers)
    : catalog(catalog), markers(markers){}

CellPredicatePtr BuildableLoader::parsePredicate(const json& j) const {
    if (!j.is_object()) throw std::runtime_error("a predicate must be a JSON object");
    const auto type = j.at("type").get<std::string>();

    if (type == "empty") {
        requireKnownKeys(j, {"type"}, "predicate 'empty'");
        return std::make_shared<EmptyCell>();
    }
    if (type == "hasMarker") {
        requireKnownKeys(j, {"type", "marker", "matchOrientation"}, "predicate 'hasMarker'");
        return std::make_shared<HasMarker>(markers.get(j.at("marker").get<std::string>()),
                                           j.value("matchOrientation", false));
    }
    if (type == "not") {
        requireKnownKeys(j, {"type", "predicate"}, "predicate 'not'");
        return std::make_shared<Not>(parsePredicate(j.at("predicate")));
    }
    if (type == "allOf") {
        requireKnownKeys(j, {"type", "predicates"}, "predicate 'allOf'");
        return std::make_shared<AllOf>(parsePredicateList(j.at("predicates")));
    }
    if (type == "anyOf") {
        requireKnownKeys(j, {"type", "predicates"}, "predicate 'anyOf'");
        return std::make_shared<AnyOf>(parsePredicateList(j.at("predicates")));
    }
    if (type == "atOffset") {
        requireKnownKeys(j, {"type", "offset", "predicate"}, "predicate 'atOffset'");
        const auto offset = readArray<3, int32_t>(j.at("offset"), "'offset'");
        return std::make_shared<AtOffset>(offset[0], offset[1], offset[2], parsePredicate(j.at("predicate")));
    }
    throw std::runtime_error("unknown predicate type '" + type + "'");
}

std::vector<CellPredicatePtr> BuildableLoader::parsePredicateList(const json& array) const {
    if (!array.is_array()) throw std::runtime_error("'predicates' must be an array");
    std::vector<CellPredicatePtr> out;
    out.reserve(array.size());
    for (const auto& entry : array) out.push_back(parsePredicate(entry));
    return out;
}

BuildableCell BuildableLoader::parseCell(const json& j) const {
    requireKnownKeys(j, {"offset", "markers", "predicate"}, "cell");

    BuildableCell cell;
    if (j.contains("offset")) {
        const auto offset = readArray<3, int32_t>(j.at("offset"), "'offset'");
        cell.dx = offset[0];
        cell.dy = offset[1];
        cell.dz = offset[2];
    }
    if (j.contains("markers")) {
        for (const auto& name : j.at("markers")) {
            cell.markers.push_back(markers.get(name.get<std::string>()));
        }
    }
    if (j.contains("predicate")) {
        cell.predicate = parsePredicate(j.at("predicate"));
    }
    return cell;
}

std::pair<size_t, BuildableOrientation> BuildableLoader::parseConfiguration(const json& buildable, const json& config) const {
    requireKnownKeys(config, {"orientation", "cells"}, "configuration");

    const auto rawOrientation = config.at("orientation").get<int64_t>();
    if (rawOrientation < 0) throw std::runtime_error("'orientation' cannot be negative");

    BuildableOrientation orientation;

    const json* cells = config.contains("cells")    ? &config.at("cells")
                      : buildable.contains("cells") ? &buildable.at("cells")
                      : nullptr;
    if (cells != nullptr) {
        if (!cells->is_array()) throw std::runtime_error("'cells' must be an array");
        for (const auto& cell : *cells) orientation.cells.push_back(parseCell(cell));
    }

    return {static_cast<size_t>(rawOrientation), std::move(orientation)};
}

BuildableDef BuildableLoader::parseBuildable(const json& j) const {
    requireKnownKeys(j, {"id", "isEntity", "playerPlaceable", "cells", "configurations"}, "buildable");

    BuildableDef def;
    def.name = j.at("id").get<std::string>();
    def.isEntity = j.value("isEntity", false);
    def.playerPlaceable = j.value("playerPlaceable", true);

    const json& configs = j.at("configurations");
    if (!configs.is_array() || configs.empty()) throw std::runtime_error("'configurations' must be a non-empty array");

    // Each configuration declares its orientation: together they must cover 0 to N-1 exactly once
    std::vector<std::optional<BuildableOrientation>> slots(configs.size());
    for (size_t i = 0; i < configs.size(); ++i) {
        try {
            auto [index, orientation] = parseConfiguration(j, configs[i]);
            if (index >= slots.size())
                throw std::runtime_error("orientation " + std::to_string(index) + " out of range: with "
                                         + std::to_string(slots.size()) + " configurations the orientations must be 0.."
                                         + std::to_string(slots.size() - 1));
            if (slots[index].has_value())
                throw std::runtime_error("duplicated orientation " + std::to_string(index));
            slots[index] = std::move(orientation);
        } catch (const std::exception& e) {
            throw std::runtime_error("configuration " + std::to_string(i) + ": " + e.what());
        }
    }

    def.orientations.reserve(slots.size());
    for (auto& slot : slots) def.orientations.push_back(std::move(*slot));
    return def;
}

void BuildableLoader::loadFromFile(const std::string& path) const {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("[BuildableLoader] Cannot open '" + path + "'");
    }

    json data;
    try {
        file >> data;
    } catch (const json::parse_error& e) {
        throw std::runtime_error("[BuildableLoader] JSON parsing error in '" + path + "': " + e.what());
    }

    try {
        requireKnownKeys(data, {"buildables"}, "file root");
    } catch (const std::exception& e) {
        throw std::runtime_error("[BuildableLoader] '" + path + "': " + e.what());
    }

    // --- Buildables ---
    if (data.contains("buildables")) {
        for (const auto& entry : data.at("buildables")) {
            std::string name = "<invalid>";
            try {
                if (entry.is_object() && entry.contains("id")) name = entry.at("id").get<std::string>();
                catalog.registerBuildable(parseBuildable(entry));
            } catch (const std::exception& e) {
                throw std::runtime_error("[BuildableLoader] '" + path + "' buildable '" + name + "': " + e.what());
            }
        }
    }

}
