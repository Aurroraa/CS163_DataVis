#pragma once

// Abstract Base Class
class State {
public:
    virtual ~State() = default;

    // 1. Initialize: Load resources (textures, fonts) when entering this state
    virtual void Init() {}

    // 2. Cleanup: Unload resources when leaving
    virtual void Cleanup() {}

    // 3. Logic: Handle mouse clicks, keyboard, data updates
    virtual void Update() = 0;

    // 4. Visuals: Draw everything to the screen
    virtual void Draw() = 0;
};