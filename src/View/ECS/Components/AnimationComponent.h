#ifndef IRONBRASS_ANIMATIONCOMPONENT_H
#define IRONBRASS_ANIMATIONCOMPONENT_H

#include "../../../Core/Animation/AnimationPlayer.h"

/**
 * ECS interface between the entity and AnimationPlayer.
 * Attachable to an entity to animate it.
 */
struct AnimationComponent {
    AnimationPlayer player;

    explicit AnimationComponent(const Animation& animation) : player(animation) {
        player.play();
    }
};

#endif //IRONBRASS_ANIMATIONCOMPONENT_H
