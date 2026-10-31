#include "KineticPreview.h"

#include <algorithm>
#include <cstdlib>
#include <unordered_set>
#include <vector>
#include "../../Behaviours/Kinetic/KineticDef.h"
#include "../../ECS/Components/KineticComponent.h"
#include "../../Map/TileCoord.h"

namespace {
    struct Candidate {
        entt::entity entity;
        KineticLinkKind kind;
        float phase;
    };

    struct Deposit {
        TileCoord cell;
        MarkerId marker;
    };

    bool sameCell(const TileCoord& a, const TileCoord& b) {
        return a.x == b.x && a.y == b.y && a.z == b.z;
    }

    const entt::entity* entityOf(const CellMarker& marker) {
        return std::get_if<entt::entity>(&marker.source);
    }
}

double KineticPreview::angle(const entt::registry& registry) const {
    if (parent == entt::null || !registry.valid(parent)) return 0.0;

    const auto* state = registry.try_get<KineticState>(parent);
    return state == nullptr ? 0.0 : ratio * state->angle + offset;
}

KineticPreview findKineticPreview(const entt::registry& registry, const OccupancyGrid& occupancy,
                                  const KineticDatabase& database, const KineticDef& def,
                                  const BuildableOrientation& orientation, const TileCoord& origin) {
    KineticPreview preview;
    preview.def = &def;

    std::vector<Candidate> candidates;

    // The links that the previewed buildable declares
    for (const KineticLink& link : def.links) {
        const CellContents* contents = occupancy.get(TileCoord{origin.x + link.dx, origin.y + link.dy, origin.z + link.dz});
        if (contents == nullptr) continue;

        for (const CellMarker& deposited : contents->markers) {
            if (deposited.marker != link.marker) continue;

            const entt::entity* other = entityOf(deposited);
            if (other == nullptr || !registry.all_of<KineticNode>(*other)) continue;
            if (!canConnect(link.kind, def, *registry.get<KineticNode>(*other).def)) continue;

            candidates.push_back({*other, link.kind, link.phase});
        }
    }

    // The links that the neighbors declare towards what the previewed buildable would deposit
    std::vector<Deposit> deposits;
    int reach = 0;
    for (const BuildableCell& cell : orientation.cells) {
        reach = std::max({reach, std::abs(cell.dx), std::abs(cell.dy), std::abs(cell.dz)});
        for (const MarkerId marker : cell.markers) deposits.push_back({absoluteCell(origin, cell), marker});
    }
    reach += database.maxLinkReach(); // a neighbor can be that far from the origin and still link to one of its cells

    std::unordered_set<entt::entity> seen;
    for (int dx = -reach; dx <= reach; ++dx) {
        for (int dy = -reach; dy <= reach; ++dy) {
            for (int dz = -reach; dz <= reach; ++dz) {
                const CellContents* contents = occupancy.get(TileCoord{origin.x + dx, origin.y + dy, origin.z + dz});
                if (contents == nullptr) continue;

                for (const CellMarker& deposited : contents->markers) {
                    const entt::entity* other = entityOf(deposited);
                    if (other == nullptr || !seen.insert(*other).second) continue;
                    if (!registry.all_of<KineticNode, TileCoord>(*other)) continue;

                    const KineticDef& otherDef = *registry.get<KineticNode>(*other).def;
                    const TileCoord& otherOrigin = registry.get<TileCoord>(*other);

                    for (const KineticLink& link : otherDef.links) {
                        const TileCoord target{otherOrigin.x + link.dx, otherOrigin.y + link.dy, otherOrigin.z + link.dz};
                        const bool hitsDeposit = std::ranges::any_of(deposits, [&](const Deposit& deposit) {
                            return sameCell(deposit.cell, target) && deposit.marker == link.marker;
                        });
                        if (hitsDeposit && canConnect(link.kind, otherDef, def)) {
                            candidates.push_back({*other, link.kind, link.phase});
                        }
                    }
                }
            }
        }
    }

    if (candidates.empty()) return preview;

    const auto oldest = std::ranges::min_element(candidates, {}, [&registry](const Candidate& c) {
        return registry.get<KineticNode>(c.entity).serial;
    });

    const KineticDef& parentDef = *registry.get<KineticNode>(oldest->entity).def;
    preview.parent = oldest->entity;
    preview.ratio = transmissionRatio(oldest->kind, parentDef, def);
    preview.offset = alignmentOffset(oldest->kind, oldest->phase, parentDef, def);
    return preview;
}