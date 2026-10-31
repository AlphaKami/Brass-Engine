#ifndef IRONBRASS_RESOURCEREF_H
#define IRONBRASS_RESOURCEREF_H

#include <cstdint>

enum class ResourceType : uint8_t {
    Item,
    Fluid,
    Gas
};

struct ResourceRef {
    ResourceType type;
    uint32_t id;
};

#endif //IRONBRASS_RESOURCEREF_H
