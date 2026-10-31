#include "CameraController.h"

void CameraController::handleInput(const sf::Event &event) {
    if (const auto* scroll = event.getIf<sf::Event::MouseWheelScrolled>()) {
        if (scroll->wheel == sf::Mouse::Wheel::Vertical) {
            this->targetZoom = std::clamp(this->targetZoom * std::pow(1.f / ZOOM_STEP, scroll->delta), MIN_ZOOM, MAX_ZOOM);
        }
    }
    if (const auto* pressed = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (pressed->button == sf::Mouse::Button::Left) {
            this->isDragging = true;
            this->prevMousePos = pressed->position;
            this->newMousePos = pressed->position;
            this->velocity = {0.f, 0.f};
        }
    }
    if (const auto* released = event.getIf<sf::Event::MouseButtonReleased>()) {
        if (released->button == sf::Mouse::Button::Left) this->isDragging = false;
    }
    if (const auto* moved = event.getIf<sf::Event::MouseMoved>()) {
        if (this->isDragging) this->newMousePos = moved->position;
    }
}

void CameraController::update(const float dt) {
    if (std::abs(this->targetZoom - this->camera.zoom()) > 0.0005f) {
        const float t = 1.f - std::exp(-ZOOM_SMOOTHING * dt);
        const float newZoom = this->camera.zoom() + (this->targetZoom - this->camera.zoom()) * t;
        this->camera.zoomAt(newZoom, sf::Mouse::getPosition(this->window));
    }
    if (this->isDragging) {
        const sf::Vector2f delta = this->camera.pixelToWorld(this->prevMousePos) - this->camera.pixelToWorld(this->newMousePos);
        this->camera.move(delta);

        if (dt > 0.f) this->velocity = delta / dt;
        this->prevMousePos = this->newMousePos;
    }
    else {
        this->camera.move(this->velocity * dt);

        this->velocity.x -= this->velocity.x * FRICTION * dt;
        this->velocity.y -= this->velocity.y * FRICTION * dt;

        if (std::abs(this->velocity.x) < 0.1f) this->velocity.x = 0.f;
        if (std::abs(this->velocity.y) < 0.1f) this->velocity.y = 0.f;
    }
}
