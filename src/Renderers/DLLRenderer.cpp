#include "../../include/Renderers/DLLRenderer.h"
#include "raylib.h"
#include <string>
#include <cmath>

// ---------------------------------------------------------
// HELPER: Bulletproof Arrowhead
// ---------------------------------------------------------
void DrawFilledArrowHead(Vector2 start, Vector2 end, Color color) {
    float arrowLen = 15.0f; // Large size
    float arrowWide = 6.0f; // Width

    // 1. Direction Vector
    float dx = end.x - start.x;
    float dy = end.y - start.y;
    float len = sqrtf(dx*dx + dy*dy);
    if (len == 0) return;

    // Normalize
    dx /= len; dy /= len;

    // 2. Calculate Corners
    // Base is back from the tip
    Vector2 base = { end.x - dx*arrowLen, end.y - dy*arrowLen };

    // Perpendicular vector for width
    Vector2 perp = { -dy * arrowWide, dx * arrowWide };

    Vector2 p1 = { base.x + perp.x, base.y + perp.y };
    Vector2 p2 = { base.x - perp.x, base.y - perp.y };

    // 3. Draw DOUBLE to prevent culling (Invisible triangle fix)
    DrawTriangle(end, p1, p2, color); // CCW
    DrawTriangle(end, p2, p1, color); // CW
}

// ---------------------------------------------------------
// HELPER: Orthogonal Line Drawer
// ---------------------------------------------------------
void DrawArrowStyle(Vector2 start, Vector2 end, Color color, bool isPrev) {
    float nodeRadius = 30.0f;
    float offset = isPrev ? -7.0f : 7.0f; // Separate lines

    bool isVertical = (fabs(start.y - end.y) > 10.0f);
    Vector2 pStart, pEnd;

    // A. CALCULATE START/END POINTS
    if (!isVertical) {
        // Horizontal
        pStart.y = start.y + offset;
        pEnd.y   = end.y + offset;
        if (end.x > start.x) { // Right
            pStart.x = start.x + nodeRadius;
            pEnd.x   = end.x - nodeRadius;
        } else { // Left
            pStart.x = start.x - nodeRadius;
            pEnd.x   = end.x + nodeRadius;
        }
    } else {
        // Vertical (Drop)
        pStart.x = start.x + offset;
        pEnd.x   = end.x + offset;
        if (end.y > start.y) { // Down
            pStart.y = start.y + nodeRadius;
            pEnd.y   = end.y - nodeRadius;
        } else { // Up
            pStart.y = start.y - nodeRadius;
            pEnd.y   = end.y + nodeRadius;
        }
    }

    // B. DRAW BLACK LINE & HEAD
    DrawLineEx(pStart, pEnd, 2.5f, color);
    DrawFilledArrowHead(pStart, pEnd, color);
}

// ---------------------------------------------------------
// MAIN DRAW LOOP
// ---------------------------------------------------------
void DLLRenderer::Draw(const AnimationState& state) {
    if (state.nodes.empty()) return;

    // 1. Draw Arrows
    for (const auto& node : state.nodes) {
        // NEXT Arrow (BLACK)
        if (node.nextNodeIndex != -1 && node.nextNodeIndex < state.nodes.size()) {
            const auto& target = state.nodes[node.nextNodeIndex];
            DrawArrowStyle({node.x, node.y}, {target.x, target.y}, BLACK, false);
        }

        // PREV Arrow (GRAY - Subtle)
        if (node.prevNodeIndex != -1 && node.prevNodeIndex < state.nodes.size()) {
            const auto& target = state.nodes[node.prevNodeIndex];
            DrawArrowStyle({node.x, node.y}, {target.x, target.y}, GRAY, true);
        }
    }

    // 2. Draw Nodes
    for (const auto& node : state.nodes) {
        DrawCircle(node.x, node.y, 32, BLACK);
        DrawCircle(node.x, node.y, 30, RAYWHITE); // White background like flowcharts
        DrawCircleLines(node.x, node.y, 30, BLACK); // Black ring

        std::string text = std::to_string(node.data);
        int textWidth = MeasureText(text.c_str(), 20);
        DrawText(text.c_str(), node.x - textWidth / 2, node.y - 10, 20, BLACK);
        DrawText(TextFormat("%d", node.id), node.x - 5, node.y + 35, 10, DARKGRAY);
    }

    DrawText(state.message.c_str(), 20, 20, 25, BLACK);
}