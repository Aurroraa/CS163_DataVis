#include "../../include/Renderers/TrieRenderer.h"
#include "raylib.h"
#include <string>
#include <unordered_map>
#include "UIHelper.h"
using namespace std;

namespace TrieRenderer {
    void Draw(const AnimationState& state, const UIConfig& config) {
        if (state.nodes.empty()) return;

        // 🌟 1. GRAB THEME COLORS[cite: 16]
        Color textCol = UIHelper::GetTextCol(config);
        Color nodeBgCol = UIHelper::GetCanvasBg(config);
        Color defaultBorderCol = UIHelper::GetPrimaryCol(config);
        Color lineCol = Fade(textCol, 0.4f);

        std::unordered_map<int, Vector2> nodeMap;
        for (const auto &node : state.nodes) {
            nodeMap[node.id] = {node.drawX, node.drawY};
        }

        // 2. Draw Branch Lines[cite: 16]
        for (const auto& node: state.nodes) {
            if (node.prevNodeIndex != -1 && nodeMap.count(node.prevNodeIndex)) {
                DrawLineEx({node.drawX, node.drawY}, nodeMap[node.prevNodeIndex], config.edgeThickness, lineCol);
            }
        }

        // 3. Draw Hollow Nodes[cite: 16]
        for (const auto& node : state.nodes) {
            Color currentBorder = node.color;
            if (currentBorder.r == BLUE.r && currentBorder.g == BLUE.g && currentBorder.b == BLUE.b) {
                currentBorder = defaultBorderCol;
            }

            // Outer Border & Inner Background
            DrawCircle(node.drawX, node.drawY, config.nodeRadius, currentBorder);
            DrawCircle(node.drawX, node.drawY, config.nodeRadius - config.edgeThickness, nodeBgCol);

            // 🌟 THE FIX: "End of Word" Marker (Draws a sleek inner ring)
            if (node.highlightIndex == 1) {
                DrawCircleLines(node.drawX, node.drawY, config.nodeRadius - config.edgeThickness - 4, currentBorder);
                DrawCircleLines(node.drawX, node.drawY, config.nodeRadius - config.edgeThickness - 5, currentBorder);
            }

            string text;
            int fontSize = config.textSize;
            if (node.data == 0) {
                text = "ROOT";
                fontSize = config.textSize * 0.6f; // Scale down ROOT text slightly
            } else {
                text = string(1, (char)node.data);
            }

            int textWidth = MeasureText(text.c_str(), fontSize);
            DrawText(text.c_str(), node.drawX - textWidth/2, node.drawY - fontSize/2, fontSize, textCol);
        }

        DrawText(state.message.c_str(), 20, 20, 25, textCol);
    }
}