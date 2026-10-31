#ifndef IRONBRASS_KINETICDEBUG_H
#define IRONBRASS_KINETICDEBUG_H

#include <ostream>
#include <iosfwd>
#include "../../Database/BuildableCatalog.h"
#include "../../ECS/Components/KineticComponent.h"
#include "../../Database/Buildable.h"
#include "../../Map/TileCoord.h"
#include "../../Behaviours/Kinetic/KineticDef.h"

namespace KineticDebug {
    inline const char* statusName(const KineticStatus status) {
        switch (status) {
            case KineticStatus::Idle: return "Idle";
            case KineticStatus::Running: return "Running";
            case KineticStatus::Jammed: return "Jammed";
        }
        return "?";
    }

    inline const char* axisName(const Axis axis) {
        switch (axis) {
            case Axis::X: return "X";
            case Axis::Y: return "Y";
            case Axis::Z: return "Z";
        }
        return "?";
    }

    inline const char* kindName(const KineticLinkKind kind) {
        switch (kind) {
            case KineticLinkKind::Coupled: return "coupled";
            case KineticLinkKind::Mesh: return "mesh";
        }
        return "?";
    }
}

/** Debug dump of the kinetic graph, one line per node. */
inline void printKineticLinks(const entt::registry& registry, const BuildableCatalog& catalog, std::ostream& out) {
    size_t nodeCount = 0, endpoints = 0;
    for (const auto entity : registry.view<KineticNode>()) {
        ++nodeCount;
        if (const auto* links = registry.try_get<KineticLinks>(entity)) endpoints += links->connections.size();
    }
    out << "[Kinetic] " << nodeCount << " node(s), " << endpoints / 2 << " connection(s)\n";

    auto describe = [&](const entt::entity e) {
        std::string text = "#" + std::to_string(entt::to_integral(e));
        if (const auto* built = registry.try_get<Buildable>(e)) {
            text = catalog.get(built->id).name + "(o" + std::to_string(built->orientation) + ")" + text;
        }
        if (const auto* pos = registry.try_get<TileCoord>(e)) {
            text += "@(" + std::to_string(pos->x) + "," + std::to_string(pos->y) + "," + std::to_string(pos->z) + ")";
        }
        return text;
    };

    for (const auto entity : registry.view<KineticNode>()) {
        const KineticDef& def = *registry.get<KineticNode>(entity).def;
        out << "  " << describe(entity) << " axis=" << KineticDebug::axisName(def.axis) << " teeth=" << def.teeth;
        if (const auto* state = registry.try_get<KineticState>(entity)) {
            out << " speed=" << state->speed << " angle=" << state->angle << " " << KineticDebug::statusName(state->status);
        }
        out << " ->";

        const auto* links = registry.try_get<KineticLinks>(entity);
        if (links == nullptr || links->connections.empty()) {
            out << " (none)";
        } else {
            for (const KineticConnection& c : links->connections)
                out << " " << describe(c.other) << "[" << KineticDebug::kindName(c.kind) << "]";
        }
        out << "\n";
    }
}

#endif //IRONBRASS_KINETICDEBUG_H
