#ifndef IRONBRASS_INPUTCONTEXTSTACK_H
#define IRONBRASS_INPUTCONTEXTSTACK_H

#include <vector>
#include "Action.h"
#include "InputContext.h"
#include "InputMapper.h"

class InputContextStack {
private:
    std::vector<InputContext*> stack;

public:
    void push(InputContext* context);
    void pop();

    void dispatch(const std::vector<ActionEvent>& events);
    void dispatchHeld(const std::vector<Action>& heldActions);
};


#endif //IRONBRASS_INPUTCONTEXTSTACK_H
