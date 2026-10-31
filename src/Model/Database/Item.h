#ifndef IRONBRASS_ITEM_H
#define IRONBRASS_ITEM_H

#include <cstdint>
#include <string>

using ItemId = std::uint32_t;

struct Item {
    ItemId id;
    std::string name;
    std::string displayName;
    float weight;
};

#endif //IRONBRASS_ITEM_H
