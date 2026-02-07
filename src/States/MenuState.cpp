#include "../../include/States/MenuState.h"
#include "../../include/raygui.h" // Assuming you have raygui setup
#include "../../include/App.h"

void MenuState::Init() {
    // Load background images or fonts here if needed
}

void MenuState::Update() {
    // Nothing special needed here for a simple menu
    // Buttons are handled in Draw() for immediate mode GUI
}

void MenuState::Draw() {
    // Title
    DrawText("CS163 Data Visualizer", 500, 100, 50, DARKBLUE);
    
    // Buttons
    if (GuiButton((Rectangle){600, 300, 200, 50}, "START")) {
        // TODO: Switch to Selection State
        // g_App->ChangeState(new SelectState());
    }

    if (GuiButton((Rectangle){600, 400, 200, 50}, "EXIT")) {
        CloseWindow();
    }
}