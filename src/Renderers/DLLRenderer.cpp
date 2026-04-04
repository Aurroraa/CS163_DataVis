#include "../../include/Renderers/DLLRenderer.h"
#include "raylib.h"
#include <string>
#include <cmath>

// Wrap everything inside the namespace to prevent Linker conflicts!
namespace DLLRenderer {

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
    // ---------------------------------------------------------
    // MAIN DRAW LOOP
    // ---------------------------------------------------------
    void Draw(const AnimationState& state, const UIConfig& config) {
        if (state.nodes.empty()) return;

        // 🌟 1. GRAB THEME COLORS
        Color textCol = config.isDarkMode ? Color{226, 215, 193, 255} : Color{40, 40, 40, 255};
        Color nodeBgCol = config.isDarkMode ? Color{35, 41, 49, 255} : Color{250, 250, 250, 255};
        Color defaultBorderCol = config.isDarkMode ? Color{162, 151, 137, 255} : Color{242, 182, 182, 255};

        // 2. Draw Arrows
        for (const auto& node : state.nodes) {
            // NEXT Arrow (Solid Theme Color)
            if (node.nextNodeIndex != -1 && node.nextNodeIndex < state.nodes.size()) {
                const auto& target = state.nodes[node.nextNodeIndex];
                DrawArrowStyle({node.x, node.y}, {target.x, target.y}, textCol, false);
            }

            // PREV Arrow (Faded Theme Color)
            if (node.prevNodeIndex != -1 && node.prevNodeIndex < state.nodes.size()) {
                const auto& target = state.nodes[node.prevNodeIndex];
                DrawArrowStyle({node.x, node.y}, {target.x, target.y}, Fade(textCol, 0.3f), true);
            }
        }

        // 3. Draw Nodes (HOLLOW STYLE)
        for (const auto& node : state.nodes) {
            Color currentBorder = node.color;
            if (currentBorder.r == BLUE.r && currentBorder.g == BLUE.g && currentBorder.b == BLUE.b) {
                currentBorder = defaultBorderCol;
            }

            // Outer Border
            DrawCircle(node.drawX, node.drawY, config.nodeRadius, currentBorder);
            // Inner Background
            DrawCircle(node.drawX, node.drawY, config.nodeRadius - config.edgeThickness, nodeBgCol);

            std::string text = std::to_string(node.data);
            int textWidth = MeasureText(text.c_str(), config.textSize);
            DrawText(text.c_str(), node.drawX - textWidth / 2, node.drawY - (config.textSize/2), config.textSize, textCol);

            // Index Text
            DrawText(TextFormat("%d", node.id), node.drawX - 5, node.drawY + config.nodeRadius + 5, 16, currentBorder);
        }

        DrawText(state.message.c_str(), 20, 20, 25, textCol);
    }

} // End of namespace DLLRenderer