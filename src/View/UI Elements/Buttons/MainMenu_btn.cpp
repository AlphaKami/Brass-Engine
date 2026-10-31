#include "MainMenu_btn.h"

MainMenu_btn::MainMenu_btn(const sf::Vector2f position, const sf::Vector2f size ,const std::string& text, int textSize, const sf::Font* font)
    : Button(position, size) , text(*font, text, textSize) {
    this->idleColor = sf::Color::Red;
    this->hoverColor = sf::Color::Green;
    this->activeColor = sf::Color::Magenta;

    this->btnBox.setPosition(position);
    this->btnBox.setSize(size);
    this->btnBox.setFillColor(this->idleColor);

    this->text.setFillColor(sf::Color::White);

    const sf::FloatRect textBounds = this->text.getGlobalBounds();
    this->text.setPosition({
        position.x + (size.x - textBounds.size.x) / 2.f - textBounds.position.x,
        position.y + (size.y - textBounds.size.y) / 2.f - textBounds.position.y
    });
}

void MainMenu_btn::draw(sf::RenderTarget &target, sf::RenderStates states) const {
    target.draw(this->btnBox, states);
    target.draw(text, states);
}

sf::FloatRect MainMenu_btn::getBounds() const {
    return this->btnBox.getGlobalBounds();
}

void MainMenu_btn::onHoverEnter() {
    this->btnBox.setFillColor(this->hoverColor);
}

void MainMenu_btn::onHoverExit() {
    this->btnBox.setFillColor(this->idleColor);
}

void MainMenu_btn::onPressEnter() {
    this->btnBox.setFillColor(this->activeColor);
}

void MainMenu_btn::onPressExit() {
    this->btnBox.setFillColor(isHovered() ? this->hoverColor : this->idleColor);
}
