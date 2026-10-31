#ifndef IRONBRASS_LAYERSTACK_H
#define IRONBRASS_LAYERSTACK_H

#include "../States/Layer.h"
#include <functional>

/**
 * Stack of layers.
 * Priority is ordered from top to bottom of the stack.
 */
class LayerStack {
public:
    enum class StackAction { Push, Pop, Clear};

    explicit LayerStack(const Context &context) : mContext(context) {}

    /**
     * Adds pending a new layer to be pushed on stack.
     * @tparam T class that extends Layer.
     */
    template <typename T> void pushLayer(){
        this->mPendingChanges.push_back({StackAction::Push, [this]{ return std::make_unique<T>(mContext); }});
    }

    /**
     * Adds pending the request to pop the layer on top.
     */
    void popLayer(){
        this->mPendingChanges.push_back({StackAction::Pop, nullptr});
    }

    /**
     * Adds pending the request to clear the layers stack.
     */
    void clearLayers(){
        this->mPendingChanges.push_back({StackAction::Clear, nullptr});
    }

    /**
     * Calls handleInput for all non-blocking layers on top.
     * @param event is the current polled event.
     */
    void handleInput(const sf::Event& event){
        for (auto it = mLayers.rbegin(); it != mLayers.rend(); ++it) {
            (*it)->handleInput(event);
            if ((*it)->blocksInputBelow()) break;
        }
        applyPendingChanges();
    }

    /**
     * Calls update for all non-blocking layers on top.
     * @param dt delta time.
     */
    void update(const float& dt){
        for (auto it = mLayers.rbegin(); it != mLayers.rend(); ++it) {
            (*it)->update(dt);
            if ((*it)->blocksUpdatesBelow()) break;
        }
        applyPendingChanges();
    }

    /**
     * Calls render for all non-blocking layers on top.
     */
    void render(){
        auto it = mLayers.begin();
        for (auto rit = mLayers.rbegin(); rit != mLayers.rend(); ++rit) {
            if ((*rit)->blocksRenderBelow()) { it = std::prev(rit.base()); break; }
        }
        for (; it != mLayers.end(); ++it)
            (*it)->render();
    }

private:
    using Factory = std::function<std::unique_ptr<Layer>()>;
    struct PendingChange { StackAction action; Factory factory; };

    Context mContext;
    std::vector<std::unique_ptr<Layer>> mLayers;
    std::vector<PendingChange> mPendingChanges;

    /**
     * Apply the actual action to the layer stack.
     * Once resolved, clears @link mPendingChanges
     */
    void applyPendingChanges(){
        for (auto& change : mPendingChanges) {
            switch (change.action) {
                case StackAction::Push:
                    mLayers.push_back(change.factory());
                    mLayers.back()->onEnter();
                    break;
                case StackAction::Pop:
                    mLayers.back()->onExit();
                    mLayers.pop_back();
                    break;
                case StackAction::Clear:
                    for (const auto& l : mLayers) l->onExit();
                    mLayers.clear();
                    break;
            }
        }
        mPendingChanges.clear();
    }

};

#endif //IRONBRASS_LAYERSTACK_H
