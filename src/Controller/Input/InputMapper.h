#ifndef IRONBRASS_INPUTMAPPER_H
#define IRONBRASS_INPUTMAPPER_H

#include <SFML/Window.hpp>
#include <string>
#include <unordered_map>
#include <vector>
#include "Action.h"

enum class ActionType { Pressed, Released };

struct ActionEvent {
    Action action;
    ActionType type;
};

class InputMapper {
private:
    std::unordered_map<sf::Keyboard::Key, Action> keyToAction;

    void loadKeybinds(const std::string& path);
    static const std::unordered_map<std::string, Action>& actionNameTable();

public:
    explicit InputMapper(const std::string& keybindsPath = "Resources/Config/keybinds.ini");

    std::vector<ActionEvent> pollEvents(sf::Window& window);
    std::vector<Action> heldActions() const;
};


#endif //IRONBRASS_INPUTMAPPER_H
