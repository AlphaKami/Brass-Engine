#include "InputMapper.h"
#include <fstream>
#include <sstream>
#include <iostream>

const std::unordered_map<std::string, Action>& InputMapper::actionNameTable() {
    static const std::unordered_map<std::string, Action> table = {
        {"Menu", Action::Menu},
        {"Interact", Action::Interact},
        {"MoveUp", Action::MoveUp},
        {"MoveDown", Action::MoveDown},
        {"MoveLeft", Action::MoveLeft},
        {"MoveRight", Action::MoveRight},
        // Register new actions here
    };
    return table;
}

void InputMapper::loadKeybinds(const std::string &path) {
    std::ifstream ifs(path);
    if (!ifs.is_open()) {
        std::cerr << "[InputMapper] Could not open file " << path << std::endl;
        return;
    }

    const auto& nameTable = actionNameTable();

    std::string line;
    while (std::getline(ifs, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();

        auto firstChar = line.find_first_not_of(" \t");
        if (firstChar == std::string::npos || line[firstChar] == '#') continue;

        std::istringstream iss(line);
        std::string actionName;
        int keyID;
        if (!(iss >> actionName >> keyID)) {
            std::cerr << "[InputMapper] Could not parse line: " << line << std::endl;
            continue;
        }

        auto nameIt = nameTable.find(actionName);
        if (nameIt == nameTable.end()) {
            std::cerr << "[InputMapper] Unknown action name: " << actionName << std::endl;
            continue;
        }

        if (keyID < 0 || keyID >= static_cast<int>(sf::Keyboard::KeyCount)) {
            std::cerr << "[InputMapper] Invalid key ID: " << keyID << std::endl;
            continue;
        }

        keyToAction[static_cast<sf::Keyboard::Key>(keyID)] = nameIt->second;
    }
}

InputMapper::InputMapper(const std::string &keybindsPath) {
    this->loadKeybinds(keybindsPath);
}

std::vector<ActionEvent> InputMapper::pollEvents(sf::Window &window) {
    std::vector<ActionEvent> events;

    while (const std::optional<sf::Event> event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
            continue;
        }

        if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            auto it = keyToAction.find(keyPressed->code);
            if (it != keyToAction.end()) {
                events.push_back({it->second, ActionType::Pressed});
            }
        } else if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>()) {
            auto it = keyToAction.find(keyReleased->code);
            if (it != keyToAction.end()) {
                events.push_back({it->second, ActionType::Released});
            }
        }
    }

    return events;
}

std::vector<Action> InputMapper::heldActions() const {
    std::vector<Action> held;

    for (const auto& [key, action] : keyToAction) {
        if (sf::Keyboard::isKeyPressed(key)) {
            held.push_back(action);
        }
    }

    return held;
}
