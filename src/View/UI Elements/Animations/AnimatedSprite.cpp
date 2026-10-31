#include "AnimatedSprite.h"

void AnimatedSprite::update(const float dt) {
    player.update(dt);
    sync();
}

void AnimatedSprite::draw(sf::RenderTarget &target, sf::RenderStates states) const {
    states.transform *= getTransform();
    target.draw(sprite, states);
}

void AnimatedSprite::sync() {
    if (const auto rect = player.currentRect()) sprite.setTextureRect(*rect);
}
