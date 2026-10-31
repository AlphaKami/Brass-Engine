#ifndef IRONBRASS_ANIMATIONATTACHSYSTEM_H
#define IRONBRASS_ANIMATIONATTACHSYSTEM_H

#include <vector>
#include <entt/entt.hpp>
#include "../../../Model/Database/Buildable.h"
#include "../Components/AnimationComponent.h"
#include "../../../Model/ECS/Components/KineticComponent.h"
#include "../../Gameplay/TileMap/BuildableVisuals.h"

/**
 * Gives an AnimationComponent to the buildable entities whose orientation has an animation.
 * The simulation doesn't know about animations: the view adds them to the entities it finds.
 * A kinetic node is paused at once: its frame depends on its angle, not on time (see updateKineticFrames).
 * Meant to be called every frame, after the simulation tick and before updateAnimations.
 */
inline void attachAnimations(entt::registry& registry, const BuildableVisuals& visuals) {
    const auto view = registry.view<const Buildable>(entt::exclude<AnimationComponent>);

    // Collected first: components can't be added while the view is being iterated
    std::vector<entt::entity> missing;
    for (const auto [entity, built] : view.each()) {
        if (visuals.get(built.id, built.orientation).animation != nullptr) missing.push_back(entity);
    }

    for (const entt::entity entity : missing) {
        const Buildable& built = registry.get<Buildable>(entity);
        auto& animation = registry.emplace<AnimationComponent>(entity, *visuals.get(built.id, built.orientation).animation);
        if (registry.all_of<KineticNode>(entity)) animation.player.pause();
    }
}

#endif //IRONBRASS_ANIMATIONATTACHSYSTEM_H
