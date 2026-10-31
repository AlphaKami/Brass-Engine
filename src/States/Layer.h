#ifndef IRONBRASS_LAYER_H
#define IRONBRASS_LAYER_H

#include "../Core/ResourceManager.h"
#include "../Model/Simulation.h"
#include "SFML/Graphics/RenderWindow.hpp"

class LayerStack;

/**
 * Context contains layer's necessary parameters.
 */
struct Context
{
    sf::RenderWindow *window;
    Simulation& sim;
    LayerStack &layerStack;
    ResourceManager &resourceManager;
};

/**
 * A layer is a subportion of the game.
 * Layers are handled by the layer stack and can or cannot block input, updates and render for the layers below.
 * Priority is order from top to bottom of layer stack.
 */
class Layer {
protected:
    Context context;

public:
    explicit Layer(const Context context) : context(context) {}
    virtual ~Layer() = default;

    /**
     * Handle input from main loop.
     * @param event is the current polled event.
     */
    virtual void handleInput(const sf::Event& event) = 0;

    /**
     * Update this layer; gets called every cycle if there isn't a blocking layer on top.
     * @param dt delta time.
     */
    virtual void update(const float& dt) = 0;

    /**
     * Render this layer; gets called every cycle if there isn't a blocking layer on top.
     */
    virtual void render() = 0;


    /**
     * Pure virtual, every layer must define a value
     * @return `true` if this layer blocks input for underneath layers, `false` elsewise.
     */
    virtual bool blocksInputBelow() = 0;

    /**
     * Pure virtual, every layer must define a value
     * @return `true` if this layer blocks updates for underneath layers, `false` elsewise.
     */
    virtual bool blocksUpdatesBelow() = 0;

    /**
     * Pure virtual, every layer must define a value
     * @return `true` if this layer blocks rendering for underneath layers, `false` elsewise.
     */
    virtual bool blocksRenderBelow()  = 0;

    virtual void onEnter() {}
    virtual void onExit()  {}

};

#endif //IRONBRASS_LAYER_H
