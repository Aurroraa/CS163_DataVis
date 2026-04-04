#include "../../include/Renderers/MinHeapRenderer.h"
#include "raylib.h"
#include <string>
#include "UIHelper.h"

using namespace std;

namespace MinHeapRenderer {
    void Draw(const AnimationState &state, const UIConfig& config) {
        if (state.nodes.empty()) return;

        // 🌟 1. GRAB THEME COLORS
        Color textCol = UIHelper::GetTextCol(config);
        Color nodeBgCol = UIHelper::GetCanvasBg(config);
        Color defaultBorderCol = UIHelper::GetPrimaryCol(config);
        Color lineCol = Fade(textCol, 0.4f);

        // 2. Draw Branch Lines[cite: 14]
        for (const auto &node : state.nodes) {
            if (node.nextNodeIndex != -1) {
                const auto &target = state.nodes[node.nextNodeIndex];
                DrawLineEx({node.x, node.y}, {target.x, target.y}, config.edgeThickness, lineCol);
            }
            if (node.prevNodeIndex != -1) {
                const auto &target = state.nodes[node.prevNodeIndex];
                DrawLineEx({node.x, node.y}, {target.x, target.y}, config.edgeThickness, lineCol);
            }
        }

        int boxSize = 55;
        int tableStartX = (GetScreenWidth() - (state.nodes.size() * boxSize)) / 2;
        int tableY = GetScreenHeight() - 340;

        // 3. Draw Nodes and Array Elements[cite: 14]
        for (const auto &node : state.nodes) {
            Color currentBorder = node.color;
            if (currentBorder.r == BLUE.r && currentBorder.g == BLUE.g && currentBorder.b == BLUE.b) {
                currentBorder = defaultBorderCol;
            }

            string text = to_string(node.data);
            string indexText = to_string(node.highlightIndex);

            // --- DRAW TREE NODE (Hollow) ---
            DrawCircle(node.drawX, node.drawY, config.nodeRadius, currentBorder);
            DrawCircle(node.drawX, node.drawY, config.nodeRadius - config.edgeThickness, nodeBgCol);

            int textWidth = MeasureText(text.c_str(), config.textSize);
            DrawText(text.c_str(), node.drawX - textWidth/2, node.drawY - (config.textSize/2), config.textSize, textCol);
            DrawText(indexText.c_str(), node.drawX - 5, node.drawY - config.nodeRadius - 20, 15, currentBorder);

            // --- DRAW ARRAY BOX (Hollow) ---
            int boxX = tableStartX + node.highlightIndex * boxSize;

            // Draw inner background and thick outer border to match the nodes!
            DrawRectangle(boxX, tableY, boxSize, boxSize, nodeBgCol);
            DrawRectangleLinesEx({(float)boxX, (float)tableY, (float)boxSize, (float)boxSize}, config.edgeThickness, currentBorder);

            int boxTextWidth = MeasureText(text.c_str(), config.textSize);
            DrawText(text.c_str(), boxX + boxSize/2 - boxTextWidth/2, tableY + boxSize/2 - config.textSize/2 + 2, config.textSize, textCol);
            DrawText(indexText.c_str(), boxX + 22, tableY + boxSize + 5, 15, currentBorder);
        }
    }
}