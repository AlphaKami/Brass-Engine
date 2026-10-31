#include "Game.h"

#include <fstream>

#include "../States/Layers/MainMenuLayer.h"

#include <iostream>

std::unique_ptr<sf::RenderWindow> Game::createWindow() {
    std::ifstream ifs("Resources/Config/window.ini");

    std::string titleStr = "Title";
    sf::VideoMode bounds = sf::VideoMode::getDesktopMode();
    unsigned framerateLimit = 120;
    bool isFullscreen = false;
    bool vsync = false;
    unsigned aaLevel = 0;

    if (ifs.is_open()) {
        std::getline(ifs, titleStr);
        ifs >> bounds.size.x >> bounds.size.y;
        ifs >> isFullscreen;
        ifs >> framerateLimit;
        ifs >> vsync;
        ifs >> aaLevel;
        ifs.close();
    } else {
        std::cerr << "[Game::createWindow] Failed to open window.ini" << std::endl;
    }

    sf::ContextSettings settings;
    settings.antiAliasingLevel = aaLevel;
    sf::State state = isFullscreen ? sf::State::Fullscreen : sf::State::Windowed;

    auto win = std::make_unique<sf::RenderWindow>(bounds, sf::String(titleStr), sf::Style::Titlebar | sf::Style::Close, state, settings);
    win->setFramerateLimit(framerateLimit);
    win->setVerticalSyncEnabled(vsync);
    win->setKeyRepeatEnabled(false);
    return win;
}

Game::Game() : window(createWindow()), layerStack(Context{ window.get(), simulation, layerStack, resourceManager }) {
    this->simulation.loadBuildables("Resources/Data/BuildSystemData/markers.json", "Resources/Data/BuildSystemData/buildables.json");
    this->simulation.loadKinetic("Resources/Data/KineticData/kinetic.json");
    this->layerStack.pushLayer<MainMenuLayer>();
}

Game::~Game() {
    this->window.reset(); //delete unique_ptr
}

void Game::updateDeltaTime() {
    this->deltaTime = this->dtClock.restart().asSeconds();
}

void Game::update() {
    while (const std::optional event = window->pollEvent())
    {
        const sf::Event& e = *event;
        if (e.is<sf::Event::Closed>()) { window->close(); }
        this->layerStack.handleInput(e);
    }

    this->layerStack.update(deltaTime);
}

void Game::render() {
    window->clear();
    this->layerStack.render();
    window->display();
}

void Game::run() {
    while (window->isOpen())
    {
        this->updateDeltaTime();
        this->update();
        this->render();
    }
}
