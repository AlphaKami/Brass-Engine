#ifndef IRONBRASS_BUILDABLEVISUALLOADER_H
#define IRONBRASS_BUILDABLEVISUALLOADER_H

#include <string>
#include "BuildableVisuals.h"
#include "../../../Model/Database/BuildableCatalog.h"

class ResourceManager;

/**
 * Reads the graphic part of buildables.json ("sprite", "animation", "origin" of every configuration).
 * The simulation part of the same file is read by BuildableLoader, which must have run first: buildables are matched by name.
 * Every error is reported as std::runtime_error with file and buildable name.
 */
class BuildableVisualLoader {
public:
    BuildableVisualLoader(const BuildableCatalog& catalog, const ResourceManager& resources)
        : catalog(catalog), resources(resources) {}

    void loadFromFile(const std::string& path, BuildableVisuals& out) const;

private:
    const BuildableCatalog& catalog;
    const ResourceManager& resources;
};

#endif //IRONBRASS_BUILDABLEVISUALLOADER_H
