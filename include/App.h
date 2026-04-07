#pragma once
#include "raylib.h"
#include "States/State.h"
#include <memory>

class App {
public:
    App();
    ~App();

    void Run();
    void ChangeState(State* newState);

    // 🌟 ADD THESE TWO LINES:
    Font mainFont;
    Font boldFont;

private:
    std::unique_ptr<State> currentState;
    State* nextState = nullptr;
};

extern App* g_App;