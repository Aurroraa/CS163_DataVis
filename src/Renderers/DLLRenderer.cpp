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
    void Draw(const AnimationState& state) {
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
            DrawCircle(node.drawX, node.drawY, 32, BLACK);

            // 🐛 FIX 1: Use node.color instead of RAYWHITE so animations show up!
            DrawCircle(node.drawX, node.drawY, 30, node.color);
            DrawCircleLines(node.drawX, node.drawY, 30, BLACK);

            std::string text = std::to_string(node.data);
            int textWidth = MeasureText(text.c_str(), 20);

            // 🐛 FIX 2: White text contrasts better against colored nodes
            DrawText(text.c_str(), node. drawX- textWidth / 2, node.drawY - 10, 20, WHITE);

            // Draw Index (Slightly larger and Red to match your Heap style)
            DrawText(TextFormat("%d", node.id), node.drawX - 5, node.drawY + 35, 15, RED);
        }

        // Draw Message
        DrawText(state.message.c_str(), 20, 20, 25, BLACK);
    }

} // End of namespace DLLRenderer