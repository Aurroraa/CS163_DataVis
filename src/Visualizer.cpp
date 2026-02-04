#include "../include/Visualizer.h" // Note the path!
#include "raylib.h"

// Required for Raygui to work
#define RAYGUI_IMPLEMENTATION
#include "../include/raygui.h"
#include "DataStructures/DoublyLinkedList.h"

Visualizer* globalApp = nullptr;

Visualizer::Visualizer() {
    currentStep = 0;
    isPlaying = false;
    playbackSpeed = 0.5f;
}

void Visualizer::init() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "CS163 Data Vis - Engine Test");
    SetTargetFPS(60);
    currentState = MENU;
    globalApp = this;

}

void Visualizer::recordState(std::string msg, int line, std::vector<VisualNode> data) {
    AnimationState state;
    state.message = msg;
    state.codeLineIndex = line;
    state.nodes = data;
    state.codeText = this->currentCodeBlock;
    history.push_back(state);
}

void Visualizer::run() {
    while (!WindowShouldClose()) {
        handleInput();

        BeginDrawing();
        ClearBackground(RAYWHITE);

        switch (currentState) {
            case MENU:
                drawMainMenu();
                break;
            case SELECT_MODE:   // <--- Added
                drawSelectMode();
                break;

            case ABOUT_US:      // <--- Added
                drawAboutUs();
                break;
            case RUNNING_DLL:
                // EXISTING LOGIC: Only draw history if we are in this mode
                if (GuiButton((Rectangle){10, 10, 80, 30}, "< Back")) {
                    currentState = SELECT_MODE;
                }

                // 2. Only draw if we have history
                if (!history.empty()) {
                    // Safety check
                    if (currentStep < 0) currentStep = 0;
                    if (currentStep >= history.size()) currentStep = history.size() - 1;

                    const AnimationState& state = history[currentStep];

                    // --- A. DRAW ARROWS ---
                    for (const auto& node : state.nodes) {
                        // Draw Next Arrow
                        if (node.nextNodeIndex != -1 && node.nextNodeIndex < state.nodes.size()) {
                            const auto& target = state.nodes[node.nextNodeIndex];
                            DrawLineEx({node.x, node.y - 10}, {target.x, target.y - 10}, 2, DARKGRAY);
                        }
                        // Draw Prev Arrow
                        if (node.prevNodeIndex != -1 && node.prevNodeIndex < state.nodes.size()) {
                            const auto& target = state.nodes[node.prevNodeIndex];
                            DrawLineEx({node.x, node.y + 10}, {target.x, target.y + 10}, 2, DARKGRAY);
                        }
                    }

                    // --- B. DRAW NODES ---
                    for (const auto& node : state.nodes) {
                        DrawCircle(node.x, node.y, 30, node.color);
                        DrawText(TextFormat("%d", node.data), node.x - 10, node.y - 10, 20, WHITE);
                    }

                    // --- C. DRAW CODE & CONTROLS ---
                    drawCodeBox(state);
                }

                drawControls(); // The Next/Prev buttons at the bottom
                break;
            case SETTINGS:
                DrawText("Settings Screen (Placeholder)", 100, 100, 30, BLACK);
                if (GuiButton((Rectangle){10, 10, 80, 30}, "< Back")) {
                    currentState = MENU;
                }
                break;
        }

        EndDrawing();
    }
    CloseWindow();
}

void Visualizer::handleInput() {
    // Keyboard shortcuts
    if (IsKeyPressed(KEY_RIGHT)) {
        if (currentStep < history.size() - 1) currentStep++;
    }
    if (IsKeyPressed(KEY_LEFT)) {
        if (currentStep > 0) currentStep--;
    }
}

void Visualizer::drawMainMenu() {
    // 1. Title Section (Big and Centered)
    DrawText("CS163 Data Visualizer", SCREEN_WIDTH/2 - 250, 150, 50, DARKBLUE);
    DrawText("University Project - Group X", SCREEN_WIDTH/2 - 100, 210, 20, GRAY);

    int btnWidth = 300;
    int btnHeight = 60; // Bigger buttons
    int centerX = SCREEN_WIDTH/2 - btnWidth/2;
    int startY = 350;
    int spacing = 80;

    // 2. The Buttons
    // Button A: Go to Selection Screen
    if (GuiButton((Rectangle){(float)centerX, (float)startY, (float)btnWidth, (float)btnHeight}, "SELECT DATA STRUCTURE")) {
        currentState = SELECT_MODE;
    }

    // Button B: Settings
    if (GuiButton((Rectangle){(float)centerX, (float)startY + spacing, (float)btnWidth, (float)btnHeight}, "SETTINGS")) {
        currentState = SETTINGS;
    }

    // Button C: About Us
    if (GuiButton((Rectangle){(float)centerX, (float)startY + spacing*2, (float)btnWidth, (float)btnHeight}, "ABOUT US")) {
        currentState = ABOUT_US;
    }

    // Button D: Exit
    if (GuiButton((Rectangle){(float)centerX, (float)startY + spacing*3, (float)btnWidth, (float)btnHeight}, "EXIT")) {
        CloseWindow();
    }
}

void Visualizer::drawSelectMode() {
    // Title
    DrawText("Choose a Data Structure", 50, 50, 40, DARKGRAY);

    // Back Button
    if (GuiButton((Rectangle){50, SCREEN_HEIGHT - 80, 120, 40}, "< Back")) {
        currentState = MENU;
    }

    // Grid Layout Variables
    int btnWidth = 250;
    int btnHeight = 150; // Big square tiles
    int gap = 30;
    int startX = 150;
    int startY = 150;

    // --- ROW 1: Linear & Trees ---

    // 1. Doubly Linked List
    if (GuiButton((Rectangle){(float)startX, (float)startY, (float)btnWidth, (float)btnHeight}, "Doubly Linked List")) {
        // Initialize DLL specifically here
        static DoublyLinkedList dll;
        history.clear(); // Reset "film roll"
        currentStep = 0;
        dll.init({10, 20, 30});
        dll.addHead(5);          // <--- Add this
        dll.addTail(99);         // <--- Add this
        dll.deleteNode(10);
        currentState = RUNNING_DLL;
    }

    // 2. Heap (Placeholder)
    if (GuiButton((Rectangle){(float)startX + btnWidth + gap, (float)startY, (float)btnWidth, (float)btnHeight}, "Heap / Priority Queue")) {
        // currentState = RUNNING_HEAP;
    }

    // 3. AVL Tree (Placeholder)
    if (GuiButton((Rectangle){(float)startX + (btnWidth + gap)*2, (float)startY, (float)btnWidth, (float)btnHeight}, "AVL Tree")) {
        // currentState = RUNNING_AVL;
    }

    // --- ROW 2: Advanced & Graphs ---
    int row2Y = startY + btnHeight + gap;

    // 4. Trie (Placeholder)
    if (GuiButton((Rectangle){(float)startX, (float)row2Y, (float)btnWidth, (float)btnHeight}, "Trie (Prefix Tree)")) {
        // currentState = RUNNING_TRIE;
    }

    // 5. MST (Placeholder)
    if (GuiButton((Rectangle){(float)startX + btnWidth + gap, (float)row2Y, (float)btnWidth, (float)btnHeight}, "Graph: MST")) {
        // currentState = RUNNING_MST;
    }

    // 6. Shortest Path (Placeholder)
    if (GuiButton((Rectangle){(float)startX + (btnWidth + gap)*2, (float)row2Y, (float)btnWidth, (float)btnHeight}, "Graph: Dijkstra")) {
        // currentState = RUNNING_DIJKSTRA;
    }
}

void Visualizer::drawAboutUs() {
    DrawText("About Us", 50, 50, 40, DARKGRAY);

    int textX = 100;
    int textY = 150;
    int spacing = 40;

    DrawText("Project: Data Structure Visualization", textX, textY, 30, BLACK);
    DrawText("Class: CS163 - 25A01", textX, textY + spacing, 25, DARKGRAY);

    DrawLine(textX, textY + spacing*2, textX + 500, textY + spacing*2, LIGHTGRAY);

    DrawText("Team Members:", textX, textY + spacing*3, 25, BLACK);
    DrawText("1. [Your Name] - [Your ID]", textX + 20, textY + spacing*4, 20, DARKGRAY);
    DrawText("2. [Member Name] - [Member ID]", textX + 20, textY + spacing*5, 20, DARKGRAY);
    DrawText("3. [Member Name] - [Member ID]", textX + 20, textY + spacing*6, 20, DARKGRAY);
    DrawText("4. [Member Name] - [Member ID]", textX + 20, textY + spacing*7, 20, DARKGRAY);

    // Back Button
    if (GuiButton((Rectangle){50, SCREEN_HEIGHT - 80, 120, 40}, "< Back")) {
        currentState = MENU;
    }
}

void Visualizer::drawControls() {
    // Simple UI Bar at bottom
    DrawRectangle(0, SCREEN_HEIGHT - 80, SCREEN_WIDTH, 80, LIGHTGRAY);
    
    // Buttons (using Raygui)
    if (GuiButton((Rectangle){100, (float)SCREEN_HEIGHT - 60, 100, 40}, "PREVIOUS")) {
        if (currentStep > 0) currentStep--;
    }
    
    // Draw Step Counter
    DrawText(TextFormat("Step: %d / %d", currentStep + 1, (int)history.size()), 
             600, SCREEN_HEIGHT - 50, 20, BLACK);

    if (GuiButton((Rectangle){300, (float)SCREEN_HEIGHT - 60, 100, 40}, "NEXT")) {
        if (currentStep < history.size() - 1) currentStep++;
    }
}

void Visualizer::setCodeLines(std::vector<std::string> lines) {
    currentCodeBlock = lines;
}

void Visualizer::drawCodeBox(const AnimationState& state) {
    int startX = SCREEN_WIDTH - 400;
    DrawRectangle(startX, 0, 400, SCREEN_HEIGHT, DARKGRAY);
    DrawRectangleLines(startX, 0, 400, SCREEN_HEIGHT, BLACK);

    DrawText("Source Code", startX + 10, 10, 20, LIGHTGRAY);

    int yOffset = 50;
    int lineHeight = 25;

    // --- CRITICAL CHANGE: Loop through state.codeText, NOT currentCodeBlock ---
    for (int i = 0; i < state.codeText.size(); i++) {
        int currentY = yOffset + (i * lineHeight);

        // Check against state.codeLineIndex
        if (i == state.codeLineIndex) {
            DrawRectangle(startX, currentY - 2, 400, lineHeight, {255, 255, 0, 150});
            DrawText(state.codeText[i].c_str(), startX + 15, currentY, 20, BLACK);
        } else {
            DrawText(state.codeText[i].c_str(), startX + 15, currentY, 20, WHITE);
        }
    }
}