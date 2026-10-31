#ifndef IRONBRASS_CAMERA_H
#define IRONBRASS_CAMERA_H

#include <SFML/Graphics.hpp>

/**
 * View-side state of the world camera: where it looks and how much it is zoomed.
 * It knows nothing about input; a controller moves it through move() and zoomAt().
 */
class Camera {
public:
    Camera(const sf::RenderWindow& window, const sf::Vector2f center)
        : window(window) {
        this->worldView.setSize(sf::Vector2f(window.getSize()));
        this->worldView.setCenter(center);
    }

    sf::Vector2f pixelToWorld(const sf::Vector2i pixel) const {
        return this->window.mapPixelToCoords(pixel, this->worldView);
    }

    sf::Vector2i worldToPixel(const sf::Vector2f world) const {
        return this->window.mapCoordsToPixel(world, this->worldView);
    }

    void move(const sf::Vector2f delta) { this->worldView.move(delta); }

    /** Zooms keeping the world point under `anchorPixel` fixed on screen. */
    void zoomAt(const float newZoom, const sf::Vector2i anchorPixel) {
        const sf::Vector2f before = pixelToWorld(anchorPixel);
        this->worldView.zoom(newZoom / this->zoomLevel);
        this->zoomLevel = newZoom;
        this->worldView.move(before - pixelToWorld(anchorPixel));
    }

    float zoom() const { return this->zoomLevel; }
    const sf::View& view() const { return this->worldView; }

private:
    const sf::RenderWindow& window;
    sf::View worldView;
    float zoomLevel = 1.f;
};

#endif //IRONBRASS_CAMERA_H
