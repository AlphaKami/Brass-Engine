#ifndef IRONBRASS_MAINMENULAYER_H
#define IRONBRASS_MAINMENULAYER_H

#include "../Layer.h"
#include "../../View/UI Elements/Buttons/Button.h"
#include <SFML/Graphics.hpp>

#include "../../View/UI Elements/Animations/AnimatedSprite.h"

class MainMenuLayer : public Layer {
public:
    explicit MainMenuLayer(const Context &context);

    void handleInput(const sf::Event& event) override;
    void update(const float& dt) override;
    void render() override;

    bool blocksInputBelow() override   { return true; }
    bool blocksUpdatesBelow() override { return true; }
    bool blocksRenderBelow() override  { return true; }

private:
    sf::Font *font; // TODO: spostare in un ResourceHolder
    sf::Text newGameText;
    sf::RectangleShape newGameButton;

    Button* testBtn;
    AnimatedSprite* testSprite;

    bool isMouseOverButton() const;
};


#endif //IRONBRASS_MAINMENULAYER_H
