#include "../include/App.h"
#include "raygui.h"

// Initialize the global pointer
App* g_App = nullptr;

App::App() {
    InitWindow(1440, 900, "CS163 - VisuAlgo Clone");
    SetTargetFPS(60);
    g_App = this;

    ChangeDirectory(GetApplicationDirectory());

    mainFont = LoadFontEx("assets/JetBrainsMono-Medium.ttf", 64, 0, 250);
    boldFont = LoadFontEx("assets/JetBrainsMonoNL-ExtraBold.ttf", 64, 0, 250);

    GuiSetFont(mainFont);

    GuiSetStyle(DEFAULT, TEXT_SIZE, 24);
    GuiSetStyle(DEFAULT, TEXT_SPACING, 1);
}

App::~App() {
    UnloadFont(mainFont);
    UnloadFont(boldFont);

    CloseWindow();
}

void App::ChangeState(State* newState) {
    nextState = newState;
}

void App::Run() {
    while (!WindowShouldClose()) {
        if (nextState != nullptr) {
            currentState.reset(nextState);
            currentState->Init();
            nextState = nullptr;
        }

        if (currentState) {
            currentState->Update();
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (currentState) {
            currentState->Draw();
        } else {
            DrawText("Loading...", 10, 10, 20, BLACK);
        }

        EndDrawing();
    }
}