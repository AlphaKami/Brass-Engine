#include "KineticSystem.h"

#include <entt/entt.hpp>
#include <algorithm>
#include <ostream>
#include <optional>
#include <unordered_map>
#include <utility>
#include <vector>
#include <iosfwd>
#include "../../BuildSystem/CellContents.h"
#include "../../Database/BuildableCatalog.h"
#include "../Components/KineticComponent.h"
#include "../../Database/Buildable.h"
#include "../../Map/TileCoord.h"
#include "../../Behaviours/Kinetic/KineticDef.h"

namespace {
    bool alreadyConnected(const KineticLinks& links, const entt::entity other) {
        return std::ranges::any_of(links.connections, [other](const KineticConnection& c) { return c.other == other; });
    }
    /** Speeds come from chains of float products: compare them with a small relative tolerance. */
    bool sameSpeed(const double a, const double b) {
        return std::abs(a - b) <= 1e-5f + 1e-4f * std::max(std::abs(a), std::abs(b));
    }
    /** Gains are products of exact ratios: only rounding errors have to be tolerated. */
    bool sameGain(const double a, const double b) {
        return std::abs(a - b) <= 1e-9 * std::max(std::abs(a), std::abs(b));
    }

    void advance(entt::registry& registry, const float dt, const bool integrate) {
        const auto& order = registry.ctx().get<KineticNetworkState>().order;

        // order lists every node after its parent, so the parent's angle is already up to date
        for (const entt::entity entity : order) {
            auto& state = registry.get<KineticState>(entity);
            const auto& drive = registry.get<KineticDrive>(entity);

            if (drive.parent == entt::null) {
                if (integrate) state.angle += static_cast<double>(state.speed) * dt;
            } else {
                state.angle = drive.ratio * registry.get<KineticState>(drive.parent).angle + drive.offset;
            }
        }
    }
}

void rebuildKineticLinks(entt::registry& registry, const OccupancyGrid& occupancy) {
    const auto view = registry.view<KineticNode, TileCoord>();
    const std::vector<entt::entity> nodes(view.begin(), view.end());

    for (const entt::entity entity : nodes) {
        registry.emplace_or_replace<KineticLinks>(entity);
    }

    for (const entt::entity entity : nodes) {
        const TileCoord& origin = registry.get<TileCoord>(entity);
        const KineticDef& def = *registry.get<KineticNode>(entity).def;

        for (const KineticLink& link : def.links) {
            const TileCoord cell{origin.x + link.dx, origin.y + link.dy, origin.z + link.dz};
            const CellContents* contents = occupancy.get(cell);
            if (contents == nullptr) continue;

            for (const CellMarker& deposited : contents->markers) {
                if (deposited.marker != link.marker) continue;

                const auto* source = std::get_if<entt::entity>(&deposited.source);
                if (source == nullptr || *source == entity) continue;

                const entt::entity other = *source;
                if (!registry.all_of<KineticNode, KineticLinks>(other)) continue;
                if (!canConnect(link.kind, def, *registry.get<KineticNode>(other).def)) continue;

                auto& mine = registry.get<KineticLinks>(entity);
                if (alreadyConnected(mine, other)) continue; // found from the other side first

                mine.connections.push_back({other, link.kind, link.phase});
                registry.get<KineticLinks>(other).connections.push_back({entity, link.kind, link.phase});
            }
        }
    }
}

KineticResolveReport resolveKineticNetworks(entt::registry& registry) {
    auto& network = registry.ctx().get<KineticNetworkState>();
    network.order.clear();

    const auto allView = registry.view<KineticNode, KineticState>();
    std::vector<entt::entity> nodes(allView.begin(), allView.end());
    std::ranges::sort(nodes, {}, [&registry](const entt::entity e) {
        return std::pair{registry.get<KineticNode>(e).serial, entt::to_integral(e)};
    });

    for (const entt::entity entity : nodes) {
        auto& state = registry.get<KineticState>(entity);
        state.speed = 0.f;
        state.status = KineticStatus::Idle;
        registry.emplace_or_replace<KineticDrive>(entity);
    }

    KineticResolveReport report;
    std::unordered_map<entt::entity, double> gain;

    for (const entt::entity reference : nodes) {
        if (gain.contains(reference)) continue;

        std::vector<entt::entity> component{reference};
        gain[reference] = 1.0;

        std::optional<double> referenceSpeed;
        bool jammed = false;

        for (size_t i = 0; i < component.size(); ++i) {
            const entt::entity current = component[i];
            const double currentGain = gain[current];
            const KineticDef& def = *registry.get<KineticNode>(current).def;

            if (const auto* source = registry.try_get<KineticSource>(current)) {
                const double implied = static_cast<double>(source->speed) / currentGain;
                if (!referenceSpeed) referenceSpeed = implied;
                else if (!sameSpeed(implied, *referenceSpeed)) jammed = true;
            }

            const auto* links = registry.try_get<KineticLinks>(current);
            if (links == nullptr) continue;

            for (const KineticConnection& connection : links->connections) {
                const KineticDef& otherDef = *registry.get<KineticNode>(connection.other).def;
                const double ratio = transmissionRatio(connection.kind, def, otherDef);

                if (!gain.contains(connection.other)) {
                    gain[connection.other] = currentGain * ratio;
                    registry.get<KineticDrive>(connection.other) = KineticDrive{
                        current, ratio, alignmentOffset(connection.kind, connection.phase, def, otherDef)};
                    component.push_back(connection.other);
                } else if (!sameGain(gain[connection.other], currentGain * ratio)) {
                    jammed = true; // a loop that doesn't close
                }
            }
        }

        if (referenceSpeed) ++report.networks;

        if (jammed) {
            for (const entt::entity entity : component) {
                registry.get<KineticState>(entity).status = KineticStatus::Jammed;
                registry.get<KineticDrive>(entity) = KineticDrive{};
            }
            report.jammedNodes += component.size();
            continue;
        }

        const double speed = referenceSpeed.value_or(0.0);
        const KineticStatus status = referenceSpeed ? KineticStatus::Running : KineticStatus::Idle;
        for (const entt::entity entity : component) {
            auto& state = registry.get<KineticState>(entity);
            state.speed = static_cast<float>(gain[entity] * speed);
            state.status = status;
            network.order.push_back(entity);
        }
        (referenceSpeed ? report.runningNodes : report.idleNodes) += component.size();
    }

    deriveKineticAngles(registry);
    return report;
}

void deriveKineticAngles(entt::registry& registry) { advance(registry, 0.f, false); }

void integrateKineticAngles(entt::registry& registry, const float dt) { advance(registry, dt, true); }