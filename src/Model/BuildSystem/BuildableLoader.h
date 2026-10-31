//
// Created by apoca on 06/10/2026.
//

#ifndef IRONBRASS_BUILDABLELOADER_H
#define IRONBRASS_BUILDABLELOADER_H

#include <string>
#include <utility>
#include <nlohmann/json.hpp>
#include "../Database/BuildableCatalog.h"
#include "MarkerRegistry.h"
#include "Predicates/ICellPredicate.h"

class ResourceManager;

/**
 * Reads buildable definitions from JSON and registers them in a BuildableCatalog.
 * Markers must already be declared in the MarkerRegistry: an unknown marker is a load error, not a new marker.
 * Every error is reported as std::runtime_error with file and buildable name.
 */
class BuildableLoader {
public:
    BuildableLoader(BuildableCatalog& catalog, const MarkerRegistry& markers);

    void loadFromFile(const std::string& path) const;

private:
    BuildableCatalog& catalog;
    const MarkerRegistry& markers;

    [[nodiscard]] CellPredicatePtr parsePredicate(const nlohmann::json& j) const;
    [[nodiscard]] std::vector<CellPredicatePtr> parsePredicateList(const nlohmann::json& array) const;
    [[nodiscard]] BuildableCell parseCell(const nlohmann::json& j) const;
    [[nodiscard]] std::pair<size_t, BuildableOrientation> parseConfiguration(const nlohmann::json& buildable, const nlohmann::json& config) const;
    [[nodiscard]] BuildableDef parseBuildable(const nlohmann::json& j) const;
};


#endif //IRONBRASS_BUILDABLELOADER_H
