#ifndef IRONBRASS_ICELLPREDICATE_H
#define IRONBRASS_ICELLPREDICATE_H

#include <memory>
#include "../../Map/TileCoord.h"
#include "../PlacementContext.h"

class ICellPredicate {
public:
    virtual ~ICellPredicate() = default;
    [[nodiscard]] virtual bool isSatisfied(const PlacementContext& ctx, const TileCoord& cellCoord) const = 0;
};

using CellPredicatePtr = std::shared_ptr<const ICellPredicate>;

#endif //IRONBRASS_ICELLPREDICATE_H
