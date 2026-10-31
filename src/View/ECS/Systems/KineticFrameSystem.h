#ifndef IRONBRASS_KINETICFRAMESYSTEM_H
#define IRONBRASS_KINETICFRAMESYSTEM_H

#include <entt/entt.hpp>
#include <algorithm>
#include <cmath>
#include "../Components/AnimationComponent.h"
#include "../../../Model/ECS/Components/KineticComponent.h"

/**
 * Index of the frame that shows `angle` (turns), for an animation of `frameCount` frames that covers `frameSpan` turns.
 * Positive angles advance the frames, negative ones go backwards; any number of turns is fine.
 */
inline size_t kineticFrameIndex(const double angle, const float frameSpan, const size_t frameCount) {
    const double phase = angle / static_cast<double>(frameSpan);
    const double fraction = phase - std::floor(phase); // in [0, 1), also for negative angles
    return std::min(static_cast<size_t>(fraction * static_cast<double>(frameCount)), frameCount - 1);
}

/**
 * Shows on each kinetic node the animation frame that matches its angle.
 * The frames of a node cover KineticDef::frameSpan turns (the minimal repetition of the sprite), so the frame is the position of the angle inside that span.
 * A positive speed makes the frames advance, a negative one makes them go backwards.
 *
 * @return true if at least a node has changed frame, so the view must be rebuilt.
 */
inline bool updateKineticFrames(entt::registry& registry) {
    bool anyChanged = false;

    registry.view<KineticNode, KineticState, AnimationComponent>().each(
        [&](const KineticNode& node, const KineticState& state, AnimationComponent& animation) {
            const size_t frameCount = animation.player.frameCount();
            if (frameCount == 0) return;

            if (animation.player.setFrame(kineticFrameIndex(state.angle, node.def->frameSpan, frameCount))) anyChanged = true;
        });
    return anyChanged;
}

#endif //IRONBRASS_KINETICFRAMESYSTEM_H
