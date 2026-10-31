#ifndef IRONBRASS_INPUTCONTEXT_H
#define IRONBRASS_INPUTCONTEXT_H

#include "Action.h"

class InputContext {
public:
    virtual ~InputContext() = default;

    virtual bool onActionPressed(Action action) { return false; }
    virtual bool onActionReleased(Action action) { return false; }
    virtual bool onActionHeld(Action action) { return false; }
};

#endif //IRONBRASS_INPUTCONTEXT_H
