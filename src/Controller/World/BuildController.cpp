#include "BuildController.h"

#include <algorithm>
#include <cmath>
#include <iostream>

BuildController::BuildController(sf::RenderWindow &window, Simulation &sim, const PlacementService &placement, const Camera &camera, const IsoMetrics &metrics, int maxBuildLevel)
    : window(window), sim(sim), placement(placement), camera(camera), metrics(metrics), maxBuildLevel(maxBuildLevel) {
}

void BuildController::handleInput(const sf::Event &event) {
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == this->keys.toggle) {
            this->buildMode = !this->buildMode;
            this->currentBuildLevel = 1;
        }
        if (this->buildMode) {
            if (keyPressed->code == this->keys.levelUp)   changeLevel(std::min(this->currentBuildLevel + 1, this->maxBuildLevel));
            if (keyPressed->code == this->keys.levelDown) changeLevel(std::max(this->currentBuildLevel - 1, 0));
            if (keyPressed->code == this->keys.rotate) {
                const BuildableDef& def = this->sim.getBuildableCatalog().get(this->selectedBuildable);
                this->selectedOrientation = static_cast<uint8_t>((this->selectedOrientation + 1) % def.orientations.size());
            }
            if (keyPressed->code == this->keys.cycle) cycleBuildable();
        }
    }

    if (const auto* pressed = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (pressed->button == sf::Mouse::Button::Left) this->clickStartPos = pressed->position;
    }
    if (const auto* released = event.getIf<sf::Event::MouseButtonReleased>()) {
        if (released->button == sf::Mouse::Button::Left && this->buildMode) {
            static constexpr int CLICK_THRESHOLD_PX = 5;
            const sf::Vector2i delta = released->position - this->clickStartPos;
            if (std::abs(delta.x) < CLICK_THRESHOLD_PX && std::abs(delta.y) < CLICK_THRESHOLD_PX) {
                tryPlaceAt(released->position);
            }
        }
    }
}

void BuildController::update() {
    if (!this->buildMode) {
        this->currentCursor.reset();
        return;
    }
    const sf::Vector2i mousePixel = sf::Mouse::getPosition(this->window);
    const TileCoord target = computeTarget(mousePixel);

    const BuildableDef& def = this->sim.getBuildableCatalog().get(this->selectedBuildable);
    const BuildableOrientation& orientation = def.orientations.at(this->selectedOrientation);

    this->currentCursor = BuildCursor{
        target, this->selectedBuildable, this->selectedOrientation,
        this->placement.canPlace(orientation, this->selectedOrientation, target),
        this->camera.pixelToWorld(mousePixel)
    };
}

TileCoord BuildController::computeTarget(const sf::Vector2i &pixelPos) const {
    const sf::Vector2f worldPos = this->camera.pixelToWorld(pixelPos);
    const sf::Vector2f gridPosF = iso::screenToGrid(worldPos, this->metrics, this->currentBuildLevel);

    return TileCoord{
        static_cast<int>(std::round(gridPosF.x)),
        static_cast<int>(std::round(gridPosF.y)),
        this->currentBuildLevel
    };
}

void BuildController::changeLevel(int newLevel) {
    if (newLevel == this->currentBuildLevel) return;

    // Move the mouse so it keeps pointing at the same spot of the grid on the new level.
    const sf::Vector2i currentPixel = sf::Mouse::getPosition(this->window);
    sf::Vector2f worldPos = this->camera.pixelToWorld(currentPixel);
    worldPos.y -= static_cast<float>(newLevel - this->currentBuildLevel) * this->metrics.levelHeight;
    sf::Mouse::setPosition(this->camera.worldToPixel(worldPos), this->window);

    this->currentBuildLevel = newLevel;
}

void BuildController::cycleBuildable() {
    const BuildableCatalog& catalog = this->sim.getBuildableCatalog();
    const size_t count = catalog.buildableCount();
    BuildableId next = this->selectedBuildable;
    for (size_t attempts = 0; attempts < count; ++attempts) {
        next = static_cast<BuildableId>((next + 1) % count);
        if (catalog.get(next).playerPlaceable) break;
    }
    this->selectedBuildable = next;
    this->selectedOrientation = 0;
}

void BuildController::tryPlaceAt(const sf::Vector2i &pixelPos) {
    const TileCoord origin = computeTarget(pixelPos);
    const bool placed = this->placement.tryPlace(this->selectedBuildable, this->selectedOrientation, origin);

    // debug printer
    std::cout << "[Build] placing at (" << origin.x << "," << origin.y << "," << origin.z
              << ") orientation " << static_cast<int>(this->selectedOrientation)
              << " -> " << (placed ? "success" : "failure") << std::endl;
}
