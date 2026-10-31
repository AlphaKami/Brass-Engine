#ifndef IRONBRASS_BUTTON_H
#define IRONBRASS_BUTTON_H

#include <functional>
#include <SFML/Graphics.hpp>

class Button {
public:
    Button(const sf::Vector2f position, const sf::Vector2f size) : position(position), size(size) {}
    virtual ~Button() = default;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const = 0;

    void handleInput(const sf::Event& event, sf::Vector2f mousePos) {
        const bool wasHovered = hovered;
        hovered = getBounds().contains(mousePos);

        if (hovered && !wasHovered) { onHoverEnter(); }
        if (!hovered && wasHovered) { onHoverExit(); }

        if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>()) {
            if (hovered && mbp->button == sf::Mouse::Button::Left) {
                pressed = true;
                onPressEnter();
            }
        }

        if (const auto* mbr = event.getIf<sf::Event::MouseButtonReleased>()) {
            if (mbr->button == sf::Mouse::Button::Left && pressed) {
                pressed = false;
                onPressExit();
                if (hovered && onClick) { onClick(); }
            }
        }
    }

    void setOnClick(std::function<void()> callback) { onClick = std::move(callback); }

protected:
    sf::Vector2f position;
    sf::Vector2f size;

    [[nodiscard]] virtual sf::FloatRect getBounds() const = 0;
    virtual void onHoverEnter() {}
    virtual void onHoverExit()  {}
    virtual void onPressEnter() {}
    virtual void onPressExit()  {}

    [[nodiscard]] bool isHovered() const { return hovered; }

private:
    bool hovered = false;
    bool pressed = false;
    std::function<void()> onClick;
};


#endif //IRONBRASS_BUTTON_H
