#ifndef IRONBRASS_BUILDCONTROLLER_H
#define IRONBRASS_BUILDCONTROLLER_H

#include "../../Model/Simulation.h"
#include "../../Model/BuildSystem/PlacementService.h"
#include "../../View/Gameplay/TileMap/BuildCursor.h"
#include "../../View/Gameplay/Camera.h"
#include "../../View/Gameplay/IsoMath.h"

#include <optional>


/** Keys used in build mode, gathered here until InputMapper is connected. */
struct BuildKeys {
    sf::Keyboard::Key toggle    = sf::Keyboard::Key::B;
    sf::Keyboard::Key levelUp   = sf::Keyboard::Key::V;
    sf::Keyboard::Key levelDown = sf::Keyboard::Key::C;
    sf::Keyboard::Key rotate    = sf::Keyboard::Key::R;
    sf::Keyboard::Key cycle     = sf::Keyboard::Key::Tab;
};

/**
 * Build mode: selection, build level, mouse picking and placement requests.
 */
class BuildController {
public:
    BuildController(sf::RenderWindow& window, Simulation& sim, const PlacementService& placement, const Camera& camera, const IsoMetrics& metrics, int maxBuildLevel);

    void handleInput(const sf::Event& event);
    /** Recomputes the cursor from the current mouse position. Call after the camera has moved. */
    void update();

    /** Empty when build mode is off. */
    [[nodiscard]] const std::optional<BuildCursor>& cursor() const { return this->currentCursor; }

private:
    sf::RenderWindow& window;
    Simulation& sim;
    const PlacementService& placement;
    const Camera& camera;
    const IsoMetrics& metrics;
    const int maxBuildLevel;
    BuildKeys keys;

    bool buildMode = false;
    int currentBuildLevel = 0;
    BuildableId selectedBuildable = 0; // hardcoded, TEMPORARY
    uint8_t selectedOrientation = 0;
    sf::Vector2i clickStartPos;
    std::optional<BuildCursor> currentCursor;

    TileCoord computeTarget(const sf::Vector2i& pixelPos) const;
    void changeLevel(int newLevel);
    void cycleBuildable(); // also temporary
    void tryPlaceAt(const sf::Vector2i& pixelPos);
};


#endif //IRONBRASS_BUILDCONTROLLER_H
