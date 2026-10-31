#ifndef IRONBRASS_ANIMATEDSPRITE_H
#define IRONBRASS_ANIMATEDSPRITE_H

#include <SFML/Graphics.hpp>
#include "../../../Core/Animation/AnimationPlayer.h"

/**
 * sprite with a built in animation functions.
 * Uses function of AnimationPlayer.
 */
class AnimatedSprite : public sf::Drawable, public sf::Transformable {
public:
    AnimatedSprite(const sf::Texture& texture, const Animation& animation)
        : sprite(texture), player(animation) { sync(); }

    AnimationPlayer& animation() { return player; }
    [[nodiscard]] const AnimationPlayer& animation() const { return player; }

    void update(const float dt);

protected:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    sf::Sprite sprite;
    AnimationPlayer player;

    void sync();
};


#endif //IRONBRASS_ANIMATEDSPRITE_H
