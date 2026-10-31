#ifndef IRONBRASS_ANIMATIONSYSTEM_H
#define IRONBRASS_ANIMATIONSYSTEM_H

#include <entt/entt.hpp>
#include "../Components/AnimationComponent.h"

/**
 * Advances all ECS entity that uses AnimationComponent
 * @param registry register of entt
 * @param dt delta time
 * @return true if at least an entity has changed frame
 */
inline bool updateAnimations(entt::registry& registry, const float dt) {
    bool anyChanged = false;
    registry.view<AnimationComponent>().each([&](AnimationComponent& anim) {
        if (anim.player.update(dt).frameChanged) anyChanged = true;
    });
    return anyChanged;
}

#endif //IRONBRASS_ANIMATIONSYSTEM_H
