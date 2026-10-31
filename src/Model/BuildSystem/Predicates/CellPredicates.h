#ifndef IRONBRASS_CELLPREDICATES_H
#define IRONBRASS_CELLPREDICATES_H

#include "ICellPredicate.h"
#include "../MarkerId.h"
#include <vector>

/** Satisfied when nothing at all has claimed or marked this cell. */
class EmptyCell final : public ICellPredicate {
public:
    [[nodiscard]] bool isSatisfied(const PlacementContext& ctx, const TileCoord& cellCoord) const override;
};

/**
 * Satisfied when some source (entity or static tile) has deposited `marker`
 * on this cell. If matchOrientation is true, that source's orientation must
 * also equal ctx.orientationIndex.
 */
class HasMarker final : public ICellPredicate {
public:
    explicit HasMarker(MarkerId marker, bool matchOrientation = false);
    [[nodiscard]] bool isSatisfied(const PlacementContext& ctx, const TileCoord& cellCoord) const override;
private:
    MarkerId marker;
    bool matchOrientation;
};

class AtOffset final : public ICellPredicate {
public:
    AtOffset(int32_t dx, int32_t dy, int32_t dz, CellPredicatePtr predicate);
    [[nodiscard]] bool isSatisfied(const PlacementContext& ctx, const TileCoord& cellCoord) const override;
private:
    int32_t dx, dy, dz;
    CellPredicatePtr predicate;
};

/** Logical AND of child predicates. Vacuously true if empty. */
class AllOf final : public ICellPredicate {
public:
    explicit AllOf(std::vector<CellPredicatePtr> children);
    [[nodiscard]] bool isSatisfied(const PlacementContext& ctx, const TileCoord& cellCoord) const override;
private:
    std::vector<CellPredicatePtr> children;
};

/** Logical OR of child predicates. Vacuously true if empty. */
class AnyOf final : public ICellPredicate {
public:
    explicit AnyOf(std::vector<CellPredicatePtr> children);
    [[nodiscard]] bool isSatisfied(const PlacementContext& ctx, const TileCoord& cellCoord) const override;
private:
    std::vector<CellPredicatePtr> children;
};


/** Logical NOT of a single child predicate. */
class Not final : public ICellPredicate {
public:
    explicit Not(CellPredicatePtr child);
    [[nodiscard]] bool isSatisfied(const PlacementContext& ctx, const TileCoord& cellCoord) const override;
private:
    CellPredicatePtr child;
};


#endif //IRONBRASS_CELLPREDICATES_H
