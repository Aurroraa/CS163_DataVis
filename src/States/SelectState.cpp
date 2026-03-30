#include "../../include/States/SelectState.h"
#include "../../include/States/MenuState.h"
#include "../../include/States/DLLState.h"
#include "../../include/States/AVLState.h"
#include "../../include/States/TrieState.h"
#include "../../include/States/GraphState.h"
#include "States/MinHeapState.h"
#include "../../include/App.h"
#include "../../include/raygui.h"
#include "DataStructures/AVLTree.h"

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

    if (GuiButton((Rectangle){ (float)startX + btnWidth + 50, (float)startY, (float)btnWidth, (float)btnHeight }, "Min Heap")) {
        g_App->ChangeState(new MinHeapState()); // Loads the Heap Screen!
    }

    if (GuiButton((Rectangle){(float)startX + btnWidth * 2 + 100, (float)startY, (float)btnWidth, (float)btnHeight}, "AVL Tree")) {
        g_App->ChangeState(new AVLState());
    }
    if (GuiButton((Rectangle){(float)startX + btnWidth/2.0f, (float)startY + btnHeight + 100, (float)btnWidth, (float)btnHeight}, "Trie")) {
        g_App->ChangeState(new TrieState());
    }
    if (GuiButton((Rectangle){(float)startX + btnWidth/2.0f + btnWidth + 50, (float)startY + btnHeight + 100, (float)btnWidth, (float)btnHeight}, "Graph")) {
        g_App->ChangeState(new GraphState());
    }
}