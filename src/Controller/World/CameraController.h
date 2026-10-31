#ifndef IRONBRASS_CAMERACONTROLLER_H
#define IRONBRASS_CAMERACONTROLLER_H

#include "../../View/Gameplay/Camera.h"

/**
 * Turns mouse input into camera movement: drag with inertia and smoothed wheel zoom.
 */
class CameraController {
public:
    CameraController(Camera& camera, const sf::RenderWindow& window) : camera(camera), window(window) {}

    void handleInput(const sf::Event& event);
    void update(float dt);

private:
    static constexpr float MIN_ZOOM = 0.1f;
    static constexpr float MAX_ZOOM = 1.f;
    static constexpr float ZOOM_STEP = 1.1f;
    static constexpr float ZOOM_SMOOTHING = 12.f;
    static constexpr float FRICTION = 5.f;

    Camera& camera;
    const sf::RenderWindow& window;

    float targetZoom = 1.f;
    bool isDragging = false;
    sf::Vector2i prevMousePos;
    sf::Vector2i newMousePos;
    sf::Vector2f velocity{0.f, 0.f};
};


#endif //IRONBRASS_CAMERACONTROLLER_H
