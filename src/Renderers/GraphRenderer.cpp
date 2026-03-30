#include "../../include/Renderers/GraphRenderer.h"
#include "raylib.h"
#include <string>
#include <unordered_map>
#include <cmath>

namespace GraphRenderer {

    void DrawArrow(Vector2 start, Vector2 end, Color color) {
        float dx = end.x - start.x;
        float dy = end.y - start.y;
        float length = sqrt(dx*dx + dy*dy);
        
        if (length > 0) {
            dx /= length; dy /= length;
            // Pull the end point back by the node's radius (30) so the arrow doesn't hide under the circle
            Vector2 realEnd = { end.x - dx * 30.0f, end.y - dy * 30.0f };
            
            DrawLineEx(start, realEnd, 3.0f, color);
            
            // Draw Arrowhead
            float arrowSize = 15.0f;
            Vector2 p1 = { realEnd.x - dx*arrowSize - dy*8.0f, realEnd.y - dy*arrowSize + dx*8.0f };
            Vector2 p2 = { realEnd.x - dx*arrowSize + dy*8.0f, realEnd.y - dy*arrowSize - dx*8.0f };
            DrawTriangle(realEnd, p1, p2, color);
            DrawTriangle(realEnd, p2, p1, color); // Draw twice for culling safety
        }
    }

    void Draw(const AnimationState& state) {
        std::unordered_map<int, Vector2> nodeMap;
        for (const auto &node : state.nodes) nodeMap[node.id] = {node.drawX, node.drawY};

       // 1. Draw Edges
        for (const auto& edge : state.edges) {
            Vector2 start = {0, 0};
            Vector2 end = {0, 0};

            for (const auto& node : state.nodes) {
                if (node.id == edge.fromId) { start.x = node.drawX; start.y = node.drawY; }
                if (node.id == edge.toId) { end.x = node.drawX; end.y = node.drawY; }
            }

            float dx = end.x - start.x;
            float dy = end.y - start.y;
            float length = std::sqrt(dx*dx + dy*dy);

            if (length > 0) {
                float dirX = dx / length;
                float dirY = dy / length;

                // 🌟 PARALLEL SHIFT: If directed, check if a reverse edge exists!
                if (state.isDirected) {
                    bool hasReverse = false;
                    for (const auto& otherEdge : state.edges) {
                        if (otherEdge.fromId == edge.toId && otherEdge.toId == edge.fromId) {
                            hasReverse = true; break;
                        }
                    }

                    // Push the line 12 pixels to the "right" so they don't overlap
                    if (hasReverse) {
                        float offset = 12.0f;
                        float nx = -dirY; // Perpendicular Normal
                        float ny = dirX;

                        start.x += nx * offset; start.y += ny * offset;
                        end.x += nx * offset;   end.y += ny * offset;

                        // Recalculate the direction after the shift
                        dx = end.x - start.x; dy = end.y - start.y;
                        length = std::sqrt(dx*dx + dy*dy);
                        dirX = dx / length; dirY = dy / length;
                    }
                }

                // Stop the line exactly at the edge of the node circles (radius 26)
                Vector2 drawStart = { start.x + dirX * 26.0f, start.y + dirY * 26.0f };
                Vector2 drawEnd = { end.x - dirX * 26.0f, end.y - dirY * 26.0f };

                DrawLineEx(drawStart, drawEnd, 4.0f, edge.color);

                // 🌟 ARROWHEAD: Draw a triangle at the end of the line
                if (state.isDirected) {
                    float arrowSize = 14.0f;
                    Vector2 p1 = drawEnd;
                    Vector2 p2 = { drawEnd.x - dirX * arrowSize - dirY * (arrowSize * 0.6f),
                                   drawEnd.y - dirY * arrowSize + dirX * (arrowSize * 0.6f) };
                    Vector2 p3 = { drawEnd.x - dirX * arrowSize + dirY * (arrowSize * 0.6f),
                                   drawEnd.y - dirY * arrowSize - dirX * (arrowSize * 0.6f) };

                    // The Fix: Draw both winding orders to defeat Raylib's backface culling!
                    DrawTriangle(p1, p2, p3, edge.color);
                    DrawTriangle(p1, p3, p2, edge.color);
                }

                // 🌟 WEIGHT TEXT (Hovering cleanly above the shifted edge)
                if (edge.weight != 0) {
                    float midX = (start.x + end.x) / 2.0f;
                    float midY = (start.y + end.y) / 2.0f;

                    float nx = -dy / length;
                    float ny = dx / length;
                    if (ny > 0) { nx = -nx; ny = -ny; }

                    float offsetAmount = 20.0f;
                    float weightX = midX + nx * offsetAmount;
                    float weightY = midY + ny * offsetAmount;

                    std::string weightText = std::to_string(edge.weight);
                    int textWidth = MeasureText(weightText.c_str(), 18);

                    DrawText(weightText.c_str(), weightX - textWidth/2, weightY - 9, 18, RED);
                }
            }
        }

        // 2. Draw Nodes
        for (const auto& node : state.nodes) {
            DrawCircle(node.drawX, node.drawY, 28, BLACK);
            DrawCircle(node.drawX, node.drawY, 26, node.color);

            // 🌟 NEW: Draw a Red Ring if the node is PINNED (isFixed)
            if (node.highlightIndex == 1) {
                DrawCircleLines(node.drawX, node.drawY, 22, RED);
                DrawCircleLines(node.drawX, node.drawY, 21, RED);
            }

            std::string text = std::to_string(node.data);
            int textWidth = MeasureText(text.c_str(), 20);
            DrawText(text.c_str(), node.drawX - textWidth/2, node.drawY - 10, 20, WHITE);
        }

        DrawText(state.message.c_str(), 20, 20, 25, BLACK);

        // 🌟 NEW: Draw the Dijkstra Tracking Table!
        if (!state.table.empty()) {
            float startX = GetScreenWidth() - 420.0f; // Right above the pseudocode
            float startY = 20.0f;
            int rowHeight = 30;
            int colWidths[] = {60, 80, 80, 60}; // Widths for V, Known, Dist, Path

            // Draw Table Background
            DrawRectangle(startX, startY, 280, state.table.size() * rowHeight, {245, 245, 245, 255});
            DrawRectangleLines(startX, startY, 280, state.table.size() * rowHeight, DARKGRAY);

            for (int r = 0; r < state.table.size(); r++) {
                float currentX = startX;

                // Highlight the Header Row
                if (r == 0) DrawRectangle(startX, startY, 280, rowHeight, {200, 200, 200, 255});

                for (int c = 0; c < state.table[r].size(); c++) {
                    // Draw cell borders
                    DrawRectangleLines(currentX, startY + r * rowHeight, colWidths[c], rowHeight, GRAY);

                    // Draw text centered in the cell
                    int textWidth = MeasureText(state.table[r][c].c_str(), 18);
                    DrawText(state.table[r][c].c_str(), currentX + (colWidths[c] - textWidth) / 2, startY + r * rowHeight + 6, 18, BLACK);

                    currentX += colWidths[c];
                }
            }
        }
        // 🌟 NEW: Draw the Final Printed Paths!
        if (!state.finalPaths.empty()) {
            float boxWidth = 350.0f;
            float boxHeight = 40.0f + (state.finalPaths.size() * 25.0f);
            float startX = 280.0f; // Place it left of the center
            float startY = 80.0f;

            // Draw a nice translucent background
            DrawRectangle(startX, startY, boxWidth, boxHeight, {255, 255, 255, 220});
            DrawRectangleLines(startX, startY, boxWidth, boxHeight, DARKGRAY);

            DrawText("Final Shortest Paths:", startX + 15, startY + 10, 20, BLUE);

            float textY = startY + 40.0f;
            for (const std::string& pathStr : state.finalPaths) {
                DrawText(pathStr.c_str(), startX + 15, textY, 18, BLACK);
                textY += 25.0f;
            }
        }
    }
}