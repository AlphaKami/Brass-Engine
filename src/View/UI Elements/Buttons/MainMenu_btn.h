#ifndef IRONBRASS_MAINMENU_BTN_H
#define IRONBRASS_MAINMENU_BTN_H

#include "Button.h"

class MainMenu_btn : public Button {
public:
    MainMenu_btn(sf::Vector2f position, sf::Vector2f size ,const std::string& text, int textSize, const sf::Font* font);

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

protected:
    sf::FloatRect getBounds() const override;
    void onHoverEnter() override;
    void onHoverExit() override;
    void onPressEnter() override;
    void onPressExit() override;

private:
    sf::RectangleShape btnBox;
    sf::Text text;

    sf::Color idleColor;
    sf::Color hoverColor;
    sf::Color activeColor;
};


#endif //IRONBRASS_MAINMENU_BTN_H
