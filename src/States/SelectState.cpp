#include "../../include/States/SelectState.h"
#include "../../include/States/MenuState.h"
#include "../../include/States/DLLState.h"
#include "States/MinHeapState.h"
#include "../../include/App.h"
#include "../../include/raygui.h"

void SelectState::Init() {
    // Load specific assets if needed
}

void SelectState::Update() {
    // Logic handled in Draw() for immediate mode GUI
}

void SelectState::Draw() {
    // 1. Title
    DrawText("Choose a Data Structure", 50, 50, 40, DARKGRAY);

    // 2. Back Button
    if (GuiButton((Rectangle){50, (float)GetScreenHeight() - 80, 100, 40}, "Back")) {
        g_App->ChangeState(new MenuState());
    }

    // 3. The Grid
    int startX = 150;
    int startY = 150;
    int btnWidth = 250;
    int btnHeight = 150;

    // Button: Doubly Linked List
    if (GuiButton((Rectangle){(float)startX, (float)startY, (float)btnWidth, (float)btnHeight}, "Doubly Linked List")) {
        g_App->ChangeState(new DLLState());
    }

    if (GuiButton((Rectangle){ 50, 180, 250, 40 }, "Min Heap")) {
        g_App->ChangeState(new MinHeapState()); // Loads the Heap Screen!
    }

    if (GuiButton((Rectangle){ 50, 300, 100, 40 }, "Back to Menu")) {
        g_App->ChangeState(new MenuState());
    }
}