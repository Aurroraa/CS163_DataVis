#include "raylib.h"

// This Define is required ONLY in one file (main.cpp) to enable the button logic
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

int main() {
    // 1. Initialize Window
    InitWindow(1280, 720, "CS163: Data Structures Project");
    SetTargetFPS(60);

    // 2. Simple State Variable (The "Data")
    int ballX = 100;

    // 3. Main Loop
    while (!WindowShouldClose()) {

        // --- UPDATE ---

        // --- DRAW ---
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw Title
        DrawText("Phase 0: Setup Complete", 20, 20, 30, DARKGRAY);

        // Draw Visualization (The Ball)
        DrawCircle(ballX, 360, 50, MAROON);

        // Draw Controls (Raygui)
        // If "Next" button is clicked, move ball right
        if (GuiButton((Rectangle){ 500, 600, 100, 40 }, "NEXT STEP")) {
            ballX += 50;
        }

        // Reset Button
        if (GuiButton((Rectangle){ 300, 600, 100, 40 }, "RESET")) {
            ballX = 100;
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}