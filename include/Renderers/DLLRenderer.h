#pragma once
#include "../Global.h" // Access to AnimationState and VisualNode

namespace DLLRenderer {
    // The only function we need: Draw the current state to the screen
    void Draw(const AnimationState& state);
}