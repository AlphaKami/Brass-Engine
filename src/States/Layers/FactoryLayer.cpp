#include "FactoryLayer.h"
#include "../../View/ECS/Systems/AnimationSystem.h"
#include "../../View/ECS/Systems/KineticFrameSystem.h"
#include "../../View/Gameplay/TileMap/BuildableVisualLoader.h"
#include "../../View/ECS/Systems/AnimationAttachSystem.h"
#include <cmath>

FactoryLayer::FactoryLayer(const Context &context)
      : Layer(context), bounds{11, 11, 10}, placementService(bounds, context.sim), metrics(32.f, 16.f, 16.f),
      camera(*context.window, {0.f, 100.f}), cameraController(camera, *context.window),
      buildController(*context.window, context.sim, placementService, camera, metrics, bounds.levels - 2) {

      const BuildableId floorId = context.sim.getBuildableCatalog().getIdByName("floor");
      const BuildableId wallId = context.sim.getBuildableCatalog().getIdByName("wall");
      const BuildableId cornerId = context.sim.getBuildableCatalog().getIdByName("wall_corner");
      constexpr uint8_t WALL_RIGHT_ORIENTATION = 0;
      constexpr uint8_t WALL_LEFT_ORIENTATION = 1;

      // Note: this loop is for testing, actual map will be loaded from files.
      for (int x = 0; x < 11; ++x) {
            for (int y = 0; y < 11; ++y) {
                  const TileCoord coord(x, y, 0);
                  context.sim.placeTile(floorId, 0, coord);
            }
      }
      for (int z = 1; z < 10; ++z) {
            for (int x = 0; x < 11; ++x) {
                  for (int y = 0; y < 11; ++y) {
                        const TileCoord coord(x, y, z);
                        if (x == 0 && y != 0) {
                              context.sim.placeTile(wallId, WALL_RIGHT_ORIENTATION, coord);
                        }
                        else if (x != 0 && y == 0) {
                              context.sim.placeTile(wallId, WALL_LEFT_ORIENTATION, coord);
                        }
                        else if (x == 0 && y == 0) {
                              context.sim.placeTile(cornerId, 0, coord);
                        }
                  }
            }
      }

      this->renderer = std::make_unique<FactoryRenderer>(*context.window, context.sim, context.resourceManager, this->metrics);
}

void FactoryLayer::handleInput(const sf::Event &event) {
      this->cameraController.handleInput(event);
      this->buildController.handleInput(event);
}

void FactoryLayer::update(const float &dt) {
      this->context.sim.tick(dt);
      this->cameraController.update(dt);
      this->buildController.update();
      this->renderer->update(dt, this->buildController.cursor());
}

void FactoryLayer::render() {
      this->renderer->render(this->camera);
}