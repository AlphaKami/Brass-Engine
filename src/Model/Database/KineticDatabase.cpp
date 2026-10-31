#include "KineticDatabase.h"

#include "../../Core/JsonUtil.h"
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <stdexcept>

namespace {
    using json = nlohmann::json;
    using jsonutil::requireKnownKeys;
    using jsonutil::readArray;

    Axis parseAxis(const std::string& name) {
        if (name == "x") return Axis::X;
        if (name == "y") return Axis::Y;
        if (name == "z") return Axis::Z;
        throw std::runtime_error("unknown axis '" + name + "' (expected \"x\", \"y\" or \"z\")");
    }

    KineticLinkKind parseLinkKind(const std::string& name) {
        if (name == "coupled") return KineticLinkKind::Coupled;
        if (name == "mesh") return KineticLinkKind::Mesh;
        throw std::runtime_error("unknown kinetic link kind '" + name + "' (expected \"coupled\" or \"mesh\")");
    }

    KineticLink parseLink(const json& j, const MarkerRegistry& markers) {
        requireKnownKeys(j, {"kind", "offset", "marker", "phase"}, "kinetic link");

        KineticLink link;
        link.kind = parseLinkKind(j.at("kind").get<std::string>());
        const auto offset = readArray<3, int32_t>(j.at("offset"), "'offset'");
        link.dx = offset[0];
        link.dy = offset[1];
        link.dz = offset[2];
        link.marker = markers.get(j.at("marker").get<std::string>());

        if (j.contains("phase")) {
            if (link.kind != KineticLinkKind::Mesh) throw std::runtime_error("'phase' is only valid on 'mesh' links");
            link.phase = j.at("phase").get<float>();
        } else {
            link.phase = link.kind == KineticLinkKind::Mesh ? 0.5f : 0.f;
        }
        return link;
    }

    // `j` is one entry of "orientations"; its keys have already been checked by the caller.
    KineticDef parseDef(const json& j, const MarkerRegistry& markers) {
        KineticDef def;
        def.axis = parseAxis(j.at("axis").get<std::string>());
        def.teeth = j.value("teeth", 0.f);
        def.frameSpan = j.value("frameSpan", 1.f);
        if (def.teeth < 0.f) throw std::runtime_error("'teeth' cannot be negative");
        if (def.frameSpan <= 0.f) throw std::runtime_error("'frameSpan' must be greater than 0");

        if (j.contains("links")) {
            const json& links = j.at("links");
            if (!links.is_array()) throw std::runtime_error("'links' must be an array");

            for (const auto& link : links) {
                const KineticLink parsed = parseLink(link, markers);
                if (parsed.kind == KineticLinkKind::Mesh && def.teeth <= 0.f)
                    throw std::runtime_error("a 'mesh' link needs 'teeth' > 0 on the node that declares it");
                def.links.push_back(parsed);
            }
        }

        if (j.contains("source")) {
            const json& source = j.at("source");
            requireKnownKeys(source, {"speed"}, "'source'");
            def.sourceSpeed = source.at("speed").get<float>();
        }
        return def;
    }
}

void KineticDatabase::loadFromFile(const std::string &path, const BuildableCatalog &catalog, const MarkerRegistry &markers) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("[KineticDatabase] Cannot open '" + path + "'");
    }

    json data;
    try {
        file >> data;
    } catch (const json::parse_error& e) {
        throw std::runtime_error("[KineticDatabase] JSON parsing error in '" + path + "': " + e.what());
    }

    try {
        requireKnownKeys(data, {"kinetic"}, "file root");
    } catch (const std::exception& e) {
        throw std::runtime_error("[KineticDatabase] '" + path + "': " + e.what());
    }
    if (!data.contains("kinetic")) return;

    for (const auto& entry : data.at("kinetic")) {
        std::string name = "<invalid>";
        try {
            requireKnownKeys(entry, {"buildable", "orientations"}, "kinetic entry");
            name = entry.at("buildable").get<std::string>();

            const BuildableId id = catalog.getIdByName(name);
            const size_t orientationCount = catalog.get(id).orientations.size();

            for (const auto& item : entry.at("orientations")) {
                int64_t index = -1;
                try {
                    requireKnownKeys(item, {"orientation", "axis", "teeth", "frameSpan", "links", "source"}, "orientation entry");
                    index = item.at("orientation").get<int64_t>();
                    if (index < 0 || static_cast<size_t>(index) >= orientationCount)
                        throw std::runtime_error("the buildable has " + std::to_string(orientationCount) + " orientation(s)");

                    const auto key = keyOf(id, static_cast<std::uint8_t>(index));
                    if (defs.contains(key)) throw std::runtime_error("defined more than once");
                    const KineticDef& def = defs.emplace(key, parseDef(item, markers)).first->second;
                    for (const KineticLink& link : def.links) {
                        linkReach = std::max({linkReach, std::abs(link.dx), std::abs(link.dy), std::abs(link.dz)});
                    }
                } catch (const std::exception& e) {
                    if (index < 0) throw;
                    throw std::runtime_error("orientation " + std::to_string(index) + ": " + e.what());
                }
            }
        } catch (const std::exception& e) {
            throw std::runtime_error("[KineticDatabase] '" + path + "' buildable '" + name + "': " + e.what());
        }
    }
}

const KineticDef * KineticDatabase::find(BuildableId id, std::uint8_t orientation) const {
    const auto it = defs.find(keyOf(id, orientation));
    return it == defs.end() ? nullptr : &it->second;
}
