#include "../../include/States/SelectState.h"
#include "../../include/States/GraphState.h"
#include "../../include/States/DLLState.h"
#include "../../include/States/AVLState.h"
#include "../../include/States/MinHeapState.h"
#include "../../include/States/TrieState.h"
#include "States/MenuState.h"
#include "../../include/App.h"
#include "../../include/Visualizer.h"
#include "../../include/UIHelper.h"
#include <string>
#include <vector>

// --- HELPER STRUCT FOR CARD DATA ---

// 🌟 ADD THESE BACK:
void SelectState::Init() {
    // Load specific assets if needed
}

void SelectState::Update() {
    // Logic handled in Draw() for immediate mode GUI
}

// ... your void SelectState::Draw() stays exactly the same below this! ...

struct DSCard {
    std::string name;
    std::string description;
    char iconType;
};

// --- FORWARD DECLARATION FOR ICON DRAWING ---
void DrawDSIcon(float x, float y, float size, char type, Color color, Color bgColor, float thickness);

// --- FUNCTION TO DRAW AN INTERACTIVE CARD ---
bool DrawDataStructureCard(Rectangle rect, DSCard card, const UIConfig& config) {
    Vector2 mouse = GetMousePosition();
    bool isHovering = CheckCollisionPointRec(mouse, rect);

    // Get Theme Colors from your palette
    Color textCol = UIHelper::GetTextCol(config);
    Color cardBgCol = GetColor(0xFFCEE3FF);
    Color outlineCol = config.isDarkMode ? Color{162, 151, 137, 255} : GetColor(0xEED9D9FF); // Fixed typo here!
    Color primaryAcc = GetColor(0xFF85BBFF);

    // Hover Lift & Shadow effect
    float shadowOffset = isHovering ? 5.0f : 2.0f;
    Color currentOutline = isHovering ? primaryAcc : outlineCol;
    float currentThickness = isHovering ? 3.0f : 1.5f;

    // Draw Shadow
    DrawRectangleRounded({rect.x + shadowOffset, rect.y + shadowOffset, rect.width, rect.height}, 0.1f, 8, Fade(BLACK, 0.1f));

    // Draw Card Background & Hollow border
    DrawRectangleRounded(rect, 0.1f, 8, currentOutline);
    DrawRectangleRounded({rect.x + currentThickness, rect.y + currentThickness, rect.width - currentThickness * 2, rect.height - currentThickness * 2}, 0.1f, 8, cardBgCol);

    float centerX = rect.x + rect.width / 2.0f;
    float iconSize = rect.height * 0.35f;

    // 1. Draw Icon
    DrawDSIcon(centerX, rect.y + rect.height * 0.3f, iconSize, card.iconType, currentOutline, cardBgCol, config.edgeThickness);

    // 2. Data Structure Name
    int nameSize = config.textSize * 1.1f;
    int tw = MeasureText(card.name.c_str(), nameSize);
    DrawText(card.name.c_str(), centerX - tw/2, rect.y + rect.height * 0.55f, nameSize, textCol);

    // 3. Start Button
    Rectangle btnRect = {centerX - 60.0f, rect.y + rect.height * 0.75f, 120.0f, 40.0f};

    if (UIHelper::DrawModernBtn(btnRect, "Start", isHovering, config) || (isHovering && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))) {
        return true;
    }

    return false;
}

// ---------------------------------------------------------
// Draw
// ---------------------------------------------------------
void SelectState::Draw() {
    // 1. Set Background to Light Off-White from palette (#F5F5F5)
    ClearBackground(GetColor(0xF5F5F5FF));

    UIConfig currentConfig;

    // --- A. THEMED HEADER BAR ---
    Color headerBg = GetColor(0x021A54FF);
    float headerH = 100.0f;
    DrawRectangle(0, 0, GetScreenWidth(), headerH, headerBg);

    // Fixed Shadow: Using standard Raylib Gradient!
    DrawRectangleGradientV(0, headerH, GetScreenWidth(), 8, Fade(BLACK, 0.2f), BLANK);

    DrawText("CS Visualizer", GetScreenWidth() / 2 - MeasureText("CS Visualizer", 40) / 2, 20, 40, WHITE);

    Color subtitleCol = Fade(WHITE, 0.7f);
    DrawText("Select Data Structure to start visualization", GetScreenWidth() / 2 - MeasureText("Select Data Structure to start visualization", 20) / 2, 65, 20, subtitleCol);

    // ... (Inside SelectState::Draw, right below the Subtitle text) ...

    // 🌟 THE FIX: Custom bright white Home button for the dark header
    Rectangle homeRect = {30, 30, 90, 40};
    Vector2 mouse = GetMousePosition();
    bool hoverHome = CheckCollisionPointRec(mouse, homeRect);

    // Draw a nice translucent white button
    DrawRectangleRounded(homeRect, 0.3f, 8, hoverHome ? Fade(WHITE, 0.3f) : Fade(WHITE, 0.1f));
    DrawRectangleRoundedLinesEx(homeRect, 0.3f, 8, 2.0f, WHITE);

    int homeTw = MeasureText("Home", 20);
    DrawText("Home", homeRect.x + 45 - homeTw/2, homeRect.y + 10, 20, WHITE);

    if (hoverHome && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        g_App->ChangeState(new MenuState());
        return; // Stop drawing this frame
    }

    // --- B. RESPONSIVE GRID LAYOUT ---
    // 🌟 REMOVED the redundant 6th card!
    std::vector<DSCard> cards = {
        {"Graph Algorithms", "Visualise Shortest Path and MST.", 'G'},
        {"Doubly Linked List", "Interactive node insertion & deletion.", 'L'},
        {"AVL Balanced Tree", "Automatic rotation visualization.", 'A'},
        {"Prefix Trie", "Interactive string insertion and search.", 'P'},
        {"Min-Heap", "Visualise heapify and priority queues.", 'H'}
    };

    int cols = 3;
    // Calculate required rows based on 5 cards
    int rows = (cards.size() + cols - 1) / cols;
    float padding = 30.0f;
    float cardW = (GetScreenWidth() - (cols + 1) * padding) / cols;
    float cardH = (GetScreenHeight() - headerH - (rows + 1) * padding) / rows;

    if (cardW > 350.0f) cardW = 350.0f;
    if (cardH > 300.0f) cardH = 300.0f;

    float gridH = rows * cardH + (rows - 1) * padding;
    float startY = headerH + padding + (GetScreenHeight() - headerH - gridH - padding * 2) / 2.0f;

    // --- C. DRAW GRID OF CARDS (WITH AUTO-CENTERING ROW 2) ---
    for (int i = 0; i < cards.size(); i++) {
        int r = i / cols;
        int c = i % cols;

        // 🌟 Auto-center logic for the bottom row!
        int cardsInThisRow = (r == rows - 1 && cards.size() % cols != 0) ? cards.size() % cols : cols;
        float rowWidth = cardsInThisRow * cardW + (cardsInThisRow - 1) * padding;
        float rowStartX = (GetScreenWidth() - rowWidth) / 2.0f;

        float cx = rowStartX + c * (cardW + padding);
        float cy = startY + r * (cardH + padding);

        if (DrawDataStructureCard({cx, cy, cardW, cardH}, cards[i], currentConfig)) {
            Visualizer::Instance().ClearHistory();

            // 🌟 Updated index mapping for 5 cards!
            if (i == 0) g_App->ChangeState(new GraphState());
            else if (i == 1) g_App->ChangeState(new DLLState());
            else if (i == 2) g_App->ChangeState(new AVLState());
            else if (i == 3) g_App->ChangeState(new TrieState());
            else if (i == 4) g_App->ChangeState(new MinHeapState());

            break;
        }
    }
    // --- C. DRAW GRID OF CARDS (WITH AUTO-CENTERING) ---
    for (int i = 0; i < cards.size(); i++) {
        int r = i / cols;
        int c = i % cols;

        // 1. Find how many cards are in THIS specific row
        int cardsInThisRow = (r == rows - 1 && cards.size() % cols != 0) ? cards.size() % cols : cols;

        // 2. Calculate the exact X starting position to center this row
        float rowWidth = cardsInThisRow * cardW + (cardsInThisRow - 1) * padding;
        float rowStartX = (GetScreenWidth() - rowWidth) / 2.0f;

        // 3. 🌟 THE FIX: Use rowStartX instead of the old startX!
        float x = rowStartX + c * (cardW + padding);
        float y = startY + r * (cardH + padding);

        if (DrawDataStructureCard({x, y, cardW, cardH}, cards[i], currentConfig)) {
            Visualizer::Instance().ClearHistory();

            // Route to the correct state
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