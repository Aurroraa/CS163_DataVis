#include "Renderers/AVLRenderer.h"
#include "raylib.h"
#include <string>
#include <unordered_map>
#include "UIHelper.h"
using namespace std;

namespace AVLRenderer {
    void Draw(const AnimationState& state, const UIConfig& config) {
        if (state.nodes.empty()) return;

        // 🌟 1. GRAB THEME COLORS[cite: 11]
        Color textCol = UIHelper::GetTextCol(config);
        Color nodeBgCol = UIHelper::GetCanvasBg(config);
        Color defaultBorderCol = UIHelper::GetPrimaryCol(config);
        Color lineCol = Fade(textCol, 0.4f);

        std::unordered_map<int, Vector2> nodeMap;
        for (const auto &node : state.nodes) {
            nodeMap[node.id] = {node.drawX, node.drawY};
        }

        // 2. Draw Branch Lines[cite: 11]
        for (const auto& node: state.nodes) {
            if (node.nextNodeIndex != -1) {
                DrawLineEx({node.drawX, node.drawY}, nodeMap[node.nextNodeIndex], config.edgeThickness, lineCol);
            }
            if (node.prevNodeIndex != -1) {
                DrawLineEx({node.drawX, node.drawY}, nodeMap[node.prevNodeIndex], config.edgeThickness, lineCol);
            }
        }

        // 3. Draw Hollow Nodes[cite: 11]
        for (const auto& node : state.nodes) {
            Color currentBorder = node.color;
            if (currentBorder.r == BLUE.r && currentBorder.g == BLUE.g && currentBorder.b == BLUE.b) {
                currentBorder = defaultBorderCol;
            }

            // Outer Border & Inner Background
            DrawCircle(node.drawX, node.drawY, config.nodeRadius, currentBorder);
            DrawCircle(node.drawX, node.drawY, config.nodeRadius - config.edgeThickness, nodeBgCol);

            string text = to_string(node.data);
            int textWidth = MeasureText(text.c_str(), config.textSize);
            DrawText(text.c_str(), node.drawX - textWidth/2, node.drawY - (config.textSize/2), config.textSize, textCol);

            // Balance Factor Text
            string bfText = to_string(node.highlightIndex);
            Color bfColor = abs(node.highlightIndex) > 1 ? RED : currentBorder;
            DrawText(bfText.c_str(), node.drawX + config.nodeRadius + 5, node.drawY - config.nodeRadius, 16, bfColor);
        }

        DrawText(state.message.c_str(), 20, 20, 25, textCol);
    }
};