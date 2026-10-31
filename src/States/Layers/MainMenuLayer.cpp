#include "MainMenuLayer.h"

#include <iostream>

#include "../../States/Layers/FactoryLayer.h"
#include "../../States/LayerStack.h"
#include "../../View/UI Elements/Buttons/MainMenu_btn.h"

MainMenuLayer::MainMenuLayer(const Context &context)
    : Layer(context), font(context.resourceManager.getFont("default")), newGameText(*font, "New Game", 20)
{
    const auto windowSize = context.window->getSize();

    this->newGameButton.setSize({200.f, 50.f});
    this->newGameButton.setFillColor(sf::Color(60, 60, 60));
    this->newGameButton.setPosition(
        {static_cast<float>(windowSize.x) / 2.f - 100.f,
        static_cast<float>(windowSize.y) / 2.f - 25.f}
    );

    this->newGameText.setFillColor(sf::Color::White);
    this->newGameText.setPosition(
        {this->newGameButton.getPosition().x + 40.f,
        this->newGameButton.getPosition().y + 12.f}
    );

    this->testBtn = new MainMenu_btn({100, 100}, {100, 50}, "Test", 24, context.resourceManager.getFont("default"));
    this->testBtn->setOnClick([this, context] {
        context.layerStack.pushLayer<FactoryLayer>();
    });

    this->testSprite = new AnimatedSprite(context.resourceManager.getAnimatedTextureSheet("KineticSet"), context.resourceManager.getAnimation("KineticSet", "Large_Gear_Vert"));
    this->testSprite->setScale({6.f, 6.f});
    this->testSprite->setPosition({400.f, 400.f});
    this->testSprite->animation().setSpeed(2.f);
    this->testSprite->animation().play();
}

void MainMenuLayer::handleInput(const sf::Event& event) {
    if (const auto* mouseButtonPressed = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mouseButtonPressed->button == sf::Mouse::Button::Left && isMouseOverButton()) {
            std::cout << "Welcome from Main Menu!" << std::endl;
        }
    }

    const sf::Vector2i pixelPos = sf::Mouse::getPosition(*this->context.window);
    const sf::Vector2f mousePos = this->context.window->mapPixelToCoords(pixelPos);
    this->testBtn->handleInput(event, mousePos);
}

void MainMenuLayer::update(const float& dt) {
    this->testSprite->update(dt);
}

void MainMenuLayer::render() {
    this->context.window->draw(this->newGameButton);
    this->context.window->draw(this->newGameText);
    this->testBtn->draw(*this->context.window, sf::RenderStates::Default);
    this->context.window->draw(*this->testSprite);
}

bool MainMenuLayer::isMouseOverButton() const {
    sf::Vector2i mousePos = sf::Mouse::getPosition(*this->context.window);
    return this->newGameButton.getGlobalBounds().contains(
        {static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)});
}