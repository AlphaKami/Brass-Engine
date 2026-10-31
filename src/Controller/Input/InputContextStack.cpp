#include "InputContextStack.h"

void InputContextStack::push(InputContext *context) {
    this->stack.push_back(context);
}

void InputContextStack::pop() {
    if (!stack.empty()) {
        stack.pop_back();
    } else {
        std::cerr << "[InputContextStack] pop on empty stack, ignored" << std::endl;
    }
}

void InputContextStack::dispatch(const std::vector<ActionEvent> &events) {
    for (const auto& event : events) {
        for (auto it = stack.rbegin(); it != stack.rend(); ++it) {
            InputContext* context = *it;
            bool handled = (event.type == ActionType::Pressed) ? context->onActionPressed(event.action) : context->onActionReleased(event.action);
            if (handled) break;
        }
    }
}

void InputContextStack::dispatchHeld(const std::vector<Action> &heldActions) {
    for (Action action : heldActions) {
        for (auto it = stack.rbegin(); it != stack.rend(); ++it) {
            if ((*it)->onActionHeld(action)) break;
        }
    }
}
