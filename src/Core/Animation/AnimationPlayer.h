#ifndef IRONBRASS_ANIMATIONPLAYER_H
#define IRONBRASS_ANIMATIONPLAYER_H

#include <cstddef>
#include <optional>
#include <SFML/Graphics/Rect.hpp>
#include "Animation.h"

/**
 * Animation player holds animation function.
 * This class can be used directly on a sprite with the class AnimatedSprite or on a ECS entity with AnimationComponent.
 */
class AnimationPlayer {
public:
    enum class PlayState { Playing, Paused, Stopped };

    struct Changes {
        bool frameChanged = false;
        bool finished = false;
    };

    explicit AnimationPlayer(const Animation& animation) : anim(&animation) {}

    void play();
    void pause();
    void stop();
    void reset();
    void setSpeed(const float multiplier) { speed = multiplier; }
    void setAnimation(const Animation& animation);
    bool setFrame(std::size_t index);

    Changes update(float dt);

    [[nodiscard]] std::optional<sf::IntRect> currentRect() const;
    [[nodiscard]] PlayState getState() const { return playState; }
    [[nodiscard]] bool isPlaying() const { return playState == PlayState::Playing; }
    [[nodiscard]] bool isFinished() const { return finished; }
    [[nodiscard]] std::size_t frameCount() const { return anim->frames.size(); }
    [[nodiscard]] std::size_t currentFrame() const { return frame; }

private:
    const Animation* anim;
    std::size_t frame = 0;
    float elapsed = 0.f;
    float speed = 1.f;
    PlayState playState = PlayState::Stopped;
    bool finished = false;
};


#endif //IRONBRASS_ANIMATIONPLAYER_H
