#ifndef IRONBRASS_GAME_H
#define IRONBRASS_GAME_H

#include <SFML/Graphics.hpp>
#include <vector>

#include "ResourceManager.h"
#include "../Controller/Input/InputContextStack.h"
#include "../States/LayerStack.h"
#include "../Model/Simulation.h"

class Game : public InputContext {
public:
    Game();
    ~Game() override;

    void updateDeltaTime();
    void update();
    void render();
    void run();

private:
    // Window
    std::unique_ptr<sf::RenderWindow> window;
    std::vector<sf::VideoMode> videoModes;
    sf::State windowState{};
    sf::ContextSettings windowSettings{};
    bool fullscreen = false;

    // Engine
    sf::Clock dtClock;
    float deltaTime = 0.0f;

    ResourceManager resourceManager;
    Simulation simulation;

    // States
    LayerStack layerStack;

    static std::unique_ptr<sf::RenderWindow> createWindow();
};

#endif //IRONBRASS_GAME_H
