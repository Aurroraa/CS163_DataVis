#include "../../include/States/MenuState.h"
#include "../../include/States/SelectState.h"
#include "../../include/App.h"
#include "raylib.h"

// --- YOUR NEW "SYNTHWAVE/SUNSET" COLOR PALETTE ---
static const Color PAL_DARK   = GetColor(0x311B58FF); // Top: Background (Deep Purple)
static const Color PAL_MID    = GetColor(0x5C347BFF); // 2nd: Button Base (Mid Purple)
static const Color PAL_PINK   = GetColor(0xDF5796FF); // 3rd: Button Hover & Accents (Hot Pink)
static const Color PAL_PEACH  = GetColor(0xFFAFA6FF); // Bottom: Title & Text (Peach)


void MenuState::Init() {}
void MenuState::Update() {}

void MenuState::Draw() {
    // 1. Fill background with the Deep Purple
    ClearBackground(PAL_DARK);

    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();
    Vector2 mouse = GetMousePosition();

    // 2. Centered Title
    const char* title = "CS VISUALIZER";
    int titleFontSize = 60;
    int titleWidth = MeasureText(title, titleFontSize);
    DrawTextEx(g_App->boldFont, title, {screenW / 2.0f - titleWidth / 2.0f, screenH * 0.3f}, titleFontSize, 2.0f, PAL_PEACH);

    // Optional subtle subtitle in pink
    const char* subtitle = "Interactive Data Structures";
    int subSize = 20;
    DrawText(subtitle, screenW / 2 - MeasureText(subtitle, subSize) / 2, screenH * 0.3f + 70, subSize, PAL_PINK);

    // --- BUTTON METRICS ---
    float btnW = 240.0f;
    float btnH = 60.0f;
    float btnX = screenW / 2.0f - btnW / 2.0f;


    // 3. START BUTTON (Mid Purple -> Hot Pink on Hover)
    Rectangle startBtn = {btnX, screenH * 0.5f, btnW, btnH};
    bool startHover = CheckCollisionPointRec(mouse, startBtn);

    DrawRectangleRounded(startBtn, 0.4f, 8, startHover ? PAL_PINK : PAL_MID);

    // Text changes to dark purple when hovered for perfect contrast
    Color startTextCol = startHover ? PAL_DARK : PAL_PEACH;
    int stTw = MeasureText("Start", 24);
    DrawText("Start", startBtn.x + btnW / 2 - stTw / 2, startBtn.y + btnH / 2 - 12, 24, startTextCol);

    if (startHover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        g_App->ChangeState(new SelectState());
        return;
    }


    // 4. EXIT BUTTON (Mid Purple -> Hot Pink on Hover)
    Rectangle exitBtn = {btnX, screenH * 0.65f, btnW, btnH};
    bool exitHover = CheckCollisionPointRec(mouse, exitBtn);

    DrawRectangleRounded(exitBtn, 0.4f, 8, exitHover ? PAL_PINK : PAL_MID);

    Color exitTextCol = exitHover ? PAL_DARK : PAL_PEACH;
    int exTw = MeasureText("Exit", 24);
    DrawText("Exit", exitBtn.x + btnW / 2 - exTw / 2, exitBtn.y + btnH / 2 - 12, 24, exitTextCol);

    if (exitHover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        CloseWindow();
    }
}