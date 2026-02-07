#pragma once
#include "raylib.h"
#include "States/State.h"
#include <memory>

class App {
private:
    std::unique_ptr<State> currentState;
    
    State* nextState = nullptr;

public:
    App();
    ~App();
    
    void Run(); // The Main Game Loop
    
    // Call this to switch screens (e.g., ChangeState(new MenuState()));
    void ChangeState(State* newState);
};


extern App* g_App;