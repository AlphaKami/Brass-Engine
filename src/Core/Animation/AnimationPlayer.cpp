#include "AnimationPlayer.h"


#include <algorithm>

void AnimationPlayer::play() {
    if (finished) reset();
    playState = PlayState::Playing;
}

void AnimationPlayer::pause() {
    if (playState == PlayState::Playing) playState = PlayState::Paused;
}

void AnimationPlayer::stop() {
    playState = PlayState::Stopped;
    reset();
}

void AnimationPlayer::reset() {
    frame = 0;
    elapsed = 0.f;
    finished = false;
}

void AnimationPlayer::setAnimation(const Animation& animation) {
    anim = &animation;
    reset();
}

bool AnimationPlayer::setFrame(const std::size_t index) {
    if (anim->frames.empty()) return false;

    const std::size_t clamped = std::min(index, anim->frames.size() - 1);
    const bool changed = clamped != frame;
    frame = clamped;
    elapsed = 0.f;
    return changed;
}

AnimationPlayer::Changes AnimationPlayer::update(const float dt) {
    Changes changes;
    if (playState != PlayState::Playing) return changes;
    if (anim->frames.empty() || anim->duration <= 0.f) return changes;

    const std::size_t before = frame;
    elapsed += dt * speed;

    while (elapsed >= anim->duration) {
        elapsed -= anim->duration;
        if (frame + 1 < anim->frames.size()) {
            ++frame;
        } else if (anim->loop) {
            frame = 0;
        } else {
            playState = PlayState::Stopped;
            finished = true;
            changes.finished = true;
            break;
        }
    }

    changes.frameChanged = (frame != before);
    return changes;
}

std::optional<sf::IntRect> AnimationPlayer::currentRect() const {
    if (anim->frames.empty()) return std::nullopt;
    return anim->frames[frame];
}