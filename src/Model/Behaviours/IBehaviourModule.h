#ifndef IRONBRASS_IBEHAVIOURMODULE_H
#define IRONBRASS_IBEHAVIOURMODULE_H

#include <entt/entt.hpp>
#include "../Database/Buildable.h"

/**
 * A behaviour that some buildables have on top of being placed.
 * Simulation calls every registered module when a buildable entity is created or removed;
 * a module decides by itself whether the buildable concerns it and, if so, attaches/updates its components.
 */
class IBehaviourModule {
public:
    virtual ~IBehaviourModule() = default;

    virtual void onAttach(entt::registry& registry) {}
    virtual void onSpawn(entt::registry& registry, entt::entity entity, const Buildable& buildable) = 0;
    virtual void onRemove(entt::registry& registry, entt::entity entity, const Buildable& buildable) {}
};

#endif //IRONBRASS_IBEHAVIOURMODULE_H
