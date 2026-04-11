#include "../../include/States/SelectState.h"
#include "../../include/States/GraphState.h"
#include "../../include/States/DLLState.h"
#include "../../include/States/AVLState.h"
#include "../../include/States/MinHeapState.h"
#include "../../include/States/TrieState.h"
#include "../../include/States/MenuState.h"
#include "../../include/App.h"
#include "../../include/Visualizer.h"
#include "../../include/UIHelper.h"
#include <string>
#include <vector>

void SelectState::Init() {}
void SelectState::Update() {}

struct DSCard {
    std::string name;
    std::string description;
    char iconType;
};

void DrawDSIcon(float x, float y, float size, char type, Color color, Color bgColor, float thickness); // Keep your existing implementation

bool DrawDataStructureCard(Rectangle rect, DSCard card, const UIConfig& config) {
    Vector2 mouse = GetMousePosition();
    bool isHovering = CheckCollisionPointRec(mouse, rect);

    Color textCol = UIHelper::GetTextCol(config);
    Color cardBgCol = GetColor(0xFFCEE3FF);
    Color outlineCol = config.isDarkMode ? Color{162, 151, 137, 255} : GetColor(0xEED9D9FF);
    Color primaryAcc = GetColor(0xFF85BBFF);

    float shadowOffset = isHovering ? 5.0f : 2.0f;
    Color currentOutline = isHovering ? primaryAcc : outlineCol;
    float currentThickness = isHovering ? 3.0f : 1.5f;

    DrawRectangleRounded({rect.x + shadowOffset, rect.y + shadowOffset, rect.width, rect.height}, 0.1f, 8, Fade(BLACK, 0.1f));
    DrawRectangleRounded(rect, 0.1f, 8, currentOutline);
    DrawRectangleRounded({rect.x + currentThickness, rect.y + currentThickness, rect.width - currentThickness * 2, rect.height - currentThickness * 2}, 0.1f, 8, cardBgCol);

    float centerX = rect.x + rect.width / 2.0f;
    float iconSize = rect.height * 0.35f;

    DrawDSIcon(centerX, rect.y + rect.height * 0.3f, iconSize, card.iconType, currentOutline, cardBgCol, config.edgeThickness);

    float nameSize = config.textSize * 1.5f;
    Vector2 nameTw = MeasureTextEx(g_App->boldFont, card.name.c_str(), nameSize, 1.0f);
    DrawTextEx(g_App->boldFont, card.name.c_str(), {centerX - nameTw.x/2.0f, rect.y + rect.height * 0.55f}, nameSize, 1.0f, textCol);

    Rectangle btnRect = {centerX - 60.0f, rect.y + rect.height * 0.75f, 120.0f, 40.0f};
    if (UIHelper::DrawModernBtn(btnRect, "Start", isHovering, config) || (isHovering && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))) {
        return true;
    }
    return false;
}

void SelectState::Draw() {
    ClearBackground(GetColor(0xF5F5F5FF));
    UIConfig currentConfig;

    // Header
    Color headerBg = GetColor(0x021A54FF);
    float headerH = 100.0f;
    DrawRectangle(0, 0, GetScreenWidth(), headerH, headerBg);
    DrawRectangleGradientV(0, headerH, GetScreenWidth(), 8, Fade(BLACK, 0.2f), BLANK);

    Vector2 titleTw = MeasureTextEx(g_App->boldFont, "CS Visualizer", 40.0f, 2.0f);
    DrawTextEx(g_App->boldFont, "CS Visualizer", {GetScreenWidth() / 2.0f - titleTw.x / 2.0f, 20.0f}, 40.0f, 2.0f, WHITE);

    Color subtitleCol = Fade(WHITE, 0.7f);
    Vector2 subTw = MeasureTextEx(g_App->mainFont, "Select Data Structure to start visualization", 20.0f, 1.0f);
    DrawTextEx(g_App->mainFont, "Select Data Structure to start visualization", {GetScreenWidth() / 2.0f - subTw.x / 2.0f, 65.0f}, 20.0f, 1.0f, subtitleCol);

    // Home Button
    Rectangle homeRect = {30, 30, 90, 40};
    Vector2 mouse = GetMousePosition();
    bool hoverHome = CheckCollisionPointRec(mouse, homeRect);

    DrawRectangleRounded(homeRect, 0.3f, 8, hoverHome ? Fade(WHITE, 0.3f) : Fade(WHITE, 0.1f));
    DrawRectangleRoundedLinesEx(homeRect, 0.3f, 8, 2.0f, WHITE);

    Vector2 homeTw = MeasureTextEx(g_App->mainFont, "Home", 20.0f, 1.0f);
    DrawTextEx(g_App->mainFont, "Home", {homeRect.x + 45.0f - homeTw.x/2.0f, homeRect.y + 20.0f - homeTw.y/2.0f}, 20.0f, 1.0f, WHITE);

    if (hoverHome && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        g_App->ChangeState(new MenuState());
        return;
    }

    // Grid Layout
    std::vector<DSCard> cards = {
        {"Graph Algorithms", "Visualise Shortest Path and MST.", 'G'},
        {"Doubly Linked List", "Interactive node insertion & deletion.", 'L'},
        {"AVL Balanced Tree", "Automatic rotation visualization.", 'A'},
        {"Prefix Trie", "Interactive string insertion and search.", 'P'},
        {"Min-Heap", "Visualise heapify and priority queues.", 'H'}
    };

    int cols = 3;
    int rows = (cards.size() + cols - 1) / cols;
    float padding = 30.0f;
    float cardW = (GetScreenWidth() - (cols + 1) * padding) / cols;
    float cardH = (GetScreenHeight() - headerH - (rows + 1) * padding) / rows;

    if (cardW > 350.0f) cardW = 350.0f;
    if (cardH > 300.0f) cardH = 300.0f;

    float gridH = rows * cardH + (rows - 1) * padding;
    float startY = headerH + padding + (GetScreenHeight() - headerH - gridH - padding * 2) / 2.0f;

    for (int i = 0; i < cards.size(); i++) {
        int r = i / cols;
        int c = i % cols;

        int cardsInThisRow = (r == rows - 1 && cards.size() % cols != 0) ? cards.size() % cols : cols;
        float rowWidth = cardsInThisRow * cardW + (cardsInThisRow - 1) * padding;
        float rowStartX = (GetScreenWidth() - rowWidth) / 2.0f;

        float cx = rowStartX + c * (cardW + padding);
        float cy = startY + r * (cardH + padding);

        if (DrawDataStructureCard({cx, cy, cardW, cardH}, cards[i], currentConfig)) {
            Visualizer::Instance().ClearHistory();
            if (i == 0) g_App->ChangeState(new GraphState());
            else if (i == 1) g_App->ChangeState(new DLLState());
            else if (i == 2) g_App->ChangeState(new AVLState());
            else if (i == 3) g_App->ChangeState(new TrieState());
            else if (i == 4) g_App->ChangeState(new MinHeapState());
            break;
        }
    }
}
// ---------------------------------------------------------
// Helper: Draw Geometric Icons
// ---------------------------------------------------------
void DrawDSIcon(float x, float y, float size, char type, Color color, Color bgColor, float thickness) {
    float r = size * 0.12f; // Node radius relative to icon size
    float offset = size * 0.25f; // Spread of the nodes

    // Draw lines FIRST so they sit behind the nodes
    switch (type) {
        case 'G': // Graph (Triangle)
            DrawLineEx({x, y - offset}, {x - offset, y + offset}, thickness, color);
            DrawLineEx({x, y - offset}, {x + offset, y + offset}, thickness, color);
            DrawLineEx({x - offset, y + offset}, {x + offset, y + offset}, thickness, color);
            break;
        case 'A': // AVL (Balanced)
        case 'H': // Heap (Complete)
        case 'T': // Generic Tree
            DrawLineEx({x, y - offset}, {x - offset, y + offset}, thickness, color);
            DrawLineEx({x, y - offset}, {x + offset, y + offset}, thickness, color);
            break;
        case 'P': // Trie
            DrawLineEx({x, y - offset}, {x, y + offset}, thickness, color);
            DrawLineEx({x, y - offset}, {x - offset, y}, thickness, color);
            DrawLineEx({x, y - offset}, {x + offset, y}, thickness, color);
            break;
        case 'L': // DLL (Lines between boxes)
            DrawLineEx({x - offset, y}, {x + offset, y}, thickness * 2, color);
            break;
    }

    // Draw Nodes SECOND so they cover the line joints cleanly
    switch (type) {
        case 'G':
        case 'A':
        case 'H':
        case 'T':
            DrawCircle(x, y - offset, r, color);
            DrawCircle(x - offset, y + offset, r, color);
            DrawCircle(x + offset, y + offset, r, color);

            // Hollow effect
            DrawCircle(x, y - offset, r - thickness, bgColor);
            DrawCircle(x - offset, y + offset, r - thickness, bgColor);
            DrawCircle(x + offset, y + offset, r - thickness, bgColor);
            break;

        case 'P': // Trie Nodes
            DrawCircle(x, y - offset, r, color);
            DrawCircle(x, y + offset, r, color);
            DrawCircle(x - offset, y, r, color);
            DrawCircle(x + offset, y, r, color);

            DrawCircle(x, y - offset, r - thickness, bgColor);
            DrawCircle(x, y + offset, r - thickness, bgColor);
            DrawCircle(x - offset, y, r - thickness, bgColor);
            DrawCircle(x + offset, y, r - thickness, bgColor);
            break;

        case 'L': // DLL Boxes
            float boxW = size * 0.25f;
            float boxH = size * 0.15f;

            // Left, Center, Right boxes
            DrawRectangle(x - offset - boxW/2, y - boxH/2, boxW, boxH, bgColor);
            DrawRectangleLinesEx({x - offset - boxW/2, y - boxH/2, boxW, boxH}, thickness, color);

            DrawRectangle(x - boxW/2, y - boxH/2, boxW, boxH, bgColor);
            DrawRectangleLinesEx({x - boxW/2, y - boxH/2, boxW, boxH}, thickness, color);

            DrawRectangle(x + offset - boxW/2, y - boxH/2, boxW, boxH, bgColor);
            DrawRectangleLinesEx({x + offset - boxW/2, y - boxH/2, boxW, boxH}, thickness, color);
            break;
    }
}