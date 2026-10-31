#include "CellPredicates.h"

#include "../../Database/Buildable.h"

bool EmptyCell::isSatisfied(const PlacementContext& ctx, const TileCoord& cellCoord) const {
    const CellContents* contents = ctx.occupancy.get(cellCoord);
    return contents == nullptr || contents->isEmpty();
}

HasMarker::HasMarker(const MarkerId marker, const bool matchOrientation)
    : marker(marker), matchOrientation(matchOrientation) {}

bool HasMarker::isSatisfied(const PlacementContext& ctx, const TileCoord& cellCoord) const {
    const CellContents* contents = ctx.occupancy.get(cellCoord);
    if (contents == nullptr) return false;

    for (const auto& entry : contents->markers) {
        if (entry.marker != marker) continue;
        if (!matchOrientation) return true;
        if (orientationOf<Buildable>(entry.source, ctx.registry) == ctx.orientationIndex) return true;
    }
    return false;
}

AtOffset::AtOffset(const int32_t dx, const int32_t dy, const int32_t dz, CellPredicatePtr predicate)
    : dx(dx), dy(dy), dz(dz), predicate(std::move(predicate)) {}

bool AtOffset::isSatisfied(const PlacementContext& ctx, const TileCoord& cellCoord) const {
    const TileCoord target{cellCoord.x + dx, cellCoord.y + dy, cellCoord.z + dz};
    return predicate->isSatisfied(ctx, target);
}

AllOf::AllOf(std::vector<CellPredicatePtr> children) : children(std::move(children)) {}

bool AllOf::isSatisfied(const PlacementContext& ctx, const TileCoord& cellCoord) const {
    for (const auto& child : children) {
        if (!child->isSatisfied(ctx, cellCoord)) return false;
    }
    return true;
}

AnyOf::AnyOf(std::vector<CellPredicatePtr> children) : children(std::move(children)) {}

bool AnyOf::isSatisfied(const PlacementContext& ctx, const TileCoord& cellCoord) const {
    for (const auto& child : children) {
        if (child->isSatisfied(ctx, cellCoord)) return true;
    }
    return false;
}

Not::Not(CellPredicatePtr child) : child(std::move(child)) {}

bool Not::isSatisfied(const PlacementContext& ctx, const TileCoord& cellCoord) const {
    return !child->isSatisfied(ctx, cellCoord);
}