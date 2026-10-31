#ifndef IRONBRASS_FACTORYLAYER_H
#define IRONBRASS_FACTORYLAYER_H

#include "../Layer.h"
#include "../../Model/Map/WorldBounds.h"
#include "../../Model/BuildSystem/PlacementService.h"
#include "../../View/Gameplay/Camera.h"
#include "../../View/States Renderers/FactoryRenderer.h"
#include "../../Controller/World/CameraController.h"
#include "../../Controller/World/BuildController.h"

#include <memory>

/**
 * Gameplay screen. Only composition: it wires model, controllers and renderer and forwards the loop calls.
 */
class FactoryLayer : public Layer {
public:
    explicit FactoryLayer(const Context& context);

    void handleInput(const sf::Event& event) override;
    void update(const float& dt) override;
    void render() override;

    bool blocksInputBelow() override   { return true; }
    bool blocksUpdatesBelow() override { return true; }
    bool blocksRenderBelow() override  { return true; }

private:
    WorldBounds bounds;
    PlacementService placementService;
    IsoMetrics metrics;

    Camera camera;
    CameraController cameraController;
    BuildController buildController;
    std::unique_ptr<FactoryRenderer> renderer; // built after the map is generated
};


#endif //IRONBRASS_FACTORYLAYER_H
