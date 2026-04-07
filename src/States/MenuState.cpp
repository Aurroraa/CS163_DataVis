#include "../../include/States/MenuState.h"
#include "../../include/States/SelectState.h"
#include "../../include/App.h"
#include "raylib.h"

static const Color PAL_DARK   = GetColor(0x311B58FF);
static const Color PAL_MID    = GetColor(0x5C347BFF);
static const Color PAL_PINK   = GetColor(0xDF5796FF);
static const Color PAL_PEACH  = GetColor(0xFFAFA6FF);

void MenuState::Init() {}
void MenuState::Update() {}

void MenuState::Draw() {
    ClearBackground(PAL_DARK);

    float screenW = GetScreenWidth();
    float screenH = GetScreenHeight();
    Vector2 mouse = GetMousePosition();

    // Title
    const char* title = "CS VISUALIZER";
    float titleFontSize = 60.0f;
    Vector2 titleSize = MeasureTextEx(g_App->boldFont, title, titleFontSize, 2.0f);
    DrawTextEx(g_App->boldFont, title, {screenW / 2.0f - titleSize.x / 2.0f, screenH * 0.3f}, titleFontSize, 2.0f, PAL_PEACH);

    // Subtitle
    const char* subtitle = "Interactive Data Structures";
    float subSize = 20.0f;
    Vector2 subTw = MeasureTextEx(g_App->mainFont, subtitle, subSize, 1.0f);
    DrawTextEx(g_App->mainFont, subtitle, {screenW / 2.0f - subTw.x / 2.0f, screenH * 0.3f + 70.0f}, subSize, 1.0f, PAL_PINK);

    // Button Metrics
    float btnW = 240.0f;
    float btnH = 60.0f;
    float btnX = screenW / 2.0f - btnW / 2.0f;

    // Start Button
    Rectangle startBtn = {btnX, screenH * 0.5f, btnW, btnH};
    bool startHover = CheckCollisionPointRec(mouse, startBtn);
    DrawRectangleRounded(startBtn, 0.4f, 8, startHover ? PAL_PINK : PAL_MID);

    Color startTextCol = startHover ? PAL_DARK : PAL_PEACH;
    Vector2 stTw = MeasureTextEx(g_App->mainFont, "Start", 24.0f, 1.0f);
    DrawTextEx(g_App->mainFont, "Start", {startBtn.x + btnW / 2.0f - stTw.x / 2.0f, startBtn.y + btnH / 2.0f - stTw.y / 2.0f}, 24.0f, 1.0f, startTextCol);

    if (startHover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        g_App->ChangeState(new SelectState());
        return;
    }

    // Exit Button
    Rectangle exitBtn = {btnX, screenH * 0.65f, btnW, btnH};
    bool exitHover = CheckCollisionPointRec(mouse, exitBtn);
    DrawRectangleRounded(exitBtn, 0.4f, 8, exitHover ? PAL_PINK : PAL_MID);

    Color exitTextCol = exitHover ? PAL_DARK : PAL_PEACH;
    Vector2 exTw = MeasureTextEx(g_App->mainFont, "Exit", 24.0f, 1.0f);
    DrawTextEx(g_App->mainFont, "Exit", {exitBtn.x + btnW / 2.0f - exTw.x / 2.0f, exitBtn.y + btnH / 2.0f - exTw.y / 2.0f}, 24.0f, 1.0f, exitTextCol);

    if (exitHover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        CloseWindow();
    }
}