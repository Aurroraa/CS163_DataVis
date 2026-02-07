#include "../include/App.h"

// Initialize the global pointer
App* g_App = nullptr;

App::App() {
    InitWindow(1440, 900, "CS163 - VisuAlgo Clone");
    SetTargetFPS(60);
    g_App = this;
}

App::~App() {
    // CloseWindow() is handled in main or automatically
    CloseWindow();
}

void App::ChangeState(State* newState) {
    // We store it in a temporary variable and switch at the start of the Update loop
    // This prevents deleting a state while it's still running code.
    nextState = newState;
}

void App::Run() {
    while (!WindowShouldClose()) {
        // 1. Handle State Switching safely
        if (nextState != nullptr) {
            currentState.reset(nextState); // Deletes old state, sets new one
            currentState->Init();
            nextState = nullptr;
        }

        // 2. Update Current Screen
        if (currentState) {
            currentState->Update();
        }
        
        // 3. Draw Current Screen
        BeginDrawing();
        ClearBackground(RAYWHITE);
        
        if (currentState) {
            currentState->Draw();
        } else {
            // Loading screen or Error if no state
            DrawText("Loading...", 10, 10, 20, BLACK);
        }
        
        EndDrawing();
    }
}