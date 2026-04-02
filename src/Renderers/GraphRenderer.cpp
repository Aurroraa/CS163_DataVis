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

    void Draw(const AnimationState& state, UIConfig config) {
        std::unordered_map<int, Vector2> nodeMap;
        for (const auto &node : state.nodes) nodeMap[node.id] = {node.drawX, node.drawY};

        Color textCol = config.isDarkMode ? Color{226, 215, 193, 255} : Color{40, 40, 40, 255};
        Color nodeBgCol = config.isDarkMode ? Color{35, 41, 49, 255} : Color{250, 250, 250, 255};
        Color defaultBorderCol = config.isDarkMode ? Color{162, 151, 137, 255} : Color{242, 182, 182, 255};
        Color defaultEdgeCol = config.isDarkMode ? Color{80, 85, 95, 255} : Color{200, 200, 200, 255}; // Softer edges

        // 1. Draw Edges
        for (const auto& edge : state.edges) {
            Vector2 start = {0, 0}; Vector2 end = {0, 0};
            for (const auto& node : state.nodes) {
                if (node.id == edge.fromId) { start.x = node.drawX; start.y = node.drawY; }
                if (node.id == edge.toId) { end.x = node.drawX; end.y = node.drawY; }
            }

            float dx = end.x - start.x; float dy = end.y - start.y;
            float length = std::sqrt(dx*dx + dy*dy);

            if (length > 0) {
                float dirX = dx / length; float dirY = dy / length;

                if (state.isDirected) {
                    bool hasReverse = false;
                    for (const auto& otherEdge : state.edges) {
                        if (otherEdge.fromId == edge.toId && otherEdge.toId == edge.fromId) { hasReverse = true; break; }
                    }
                    if (hasReverse) {
                        float offset = 12.0f;
                        float nx = -dirY; float ny = dirX;
                        start.x += nx * offset; start.y += ny * offset;
                        end.x += nx * offset;   end.y += ny * offset;
                        dx = end.x - start.x; dy = end.y - start.y;
                        length = std::sqrt(dx*dx + dy*dy);
                        dirX = dx / length; dirY = dy / length;
                    }
                }

                // Stop edge at dynamic node radius
                Vector2 drawStart = { start.x + dirX * config.nodeRadius, start.y + dirY * config.nodeRadius };
                Vector2 drawEnd = { end.x - dirX * config.nodeRadius, end.y - dirY * config.nodeRadius };

                Color currentEdgeCol = edge.color;
                // If it is the default DARKGRAY, swap it for our palette-friendly edge color
                if (currentEdgeCol.r == DARKGRAY.r && currentEdgeCol.g == DARKGRAY.g && currentEdgeCol.b == DARKGRAY.b) {
                    currentEdgeCol = defaultEdgeCol;
                }
                // Draw Dynamic Edge
                DrawLineEx(drawStart, drawEnd, config.edgeThickness, currentEdgeCol);

                if (state.isDirected) {
                    float arrowSize = 14.0f + (config.edgeThickness * 0.5f);
                    Vector2 p1 = drawEnd;
                    Vector2 p2 = { drawEnd.x - dirX * arrowSize - dirY * (arrowSize * 0.6f), drawEnd.y - dirY * arrowSize + dirX * (arrowSize * 0.6f) };
                    Vector2 p3 = { drawEnd.x - dirX * arrowSize + dirY * (arrowSize * 0.6f), drawEnd.y - dirY * arrowSize - dirX * (arrowSize * 0.6f) };
                    DrawTriangle(p1, p2, p3, edge.color);
                    DrawTriangle(p1, p3, p2, edge.color);
                }

                if (edge.weight != 0) {
                    float midX = (start.x + end.x) / 2.0f; float midY = (start.y + end.y) / 2.0f;
                    float nx = -dy / length; float ny = dx / length;
                    if (ny > 0) { nx = -nx; ny = -ny; }

                    float weightX = midX + nx * 20.0f; float weightY = midY + ny * 20.0f;
                    std::string weightText = std::to_string(edge.weight);
                    int tw = MeasureText(weightText.c_str(), config.textSize);
                    DrawText(weightText.c_str(), weightX - tw/2, weightY - (config.textSize/2), config.textSize, RED);
                }
            }
        }

        // 2. Draw Nodes (🌟 NEW HOLLOW STYLE)
        for (const auto& node : state.nodes) {
            Color currentBorder = node.color;

            // If the algorithm isn't highlighting it (it's default BLUE), use our palette!
            if (currentBorder.r == BLUE.r && currentBorder.g == BLUE.g && currentBorder.b == BLUE.b) {
                currentBorder = defaultBorderCol;
            }

            // 🌟 THE FIX: If Pinned, just change the main border color, no extra rings!
            if (node.highlightIndex == 1) {
                currentBorder = RED; // Use bright red to clearly show it's locked
            }

            // Outer colored border (The highlight!)
            DrawCircle(node.drawX, node.drawY, config.nodeRadius, currentBorder);

            // Inner constant background
            DrawCircle(node.drawX, node.drawY, config.nodeRadius - config.edgeThickness, nodeBgCol);

            std::string text = std::to_string(node.data);
            int tw = MeasureText(text.c_str(), config.textSize);
            DrawText(text.c_str(), node.drawX - tw/2, node.drawY - (config.textSize/2), config.textSize, textCol);
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