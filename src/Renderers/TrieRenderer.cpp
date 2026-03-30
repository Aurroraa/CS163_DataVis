#include "../../include/Renderers/TrieRenderer.h"
#include "raylib.h"
#include <string>
#include <unordered_map>
using namespace std;

namespace TrieRenderer {
    void Draw(const AnimationState& state) {
        if (state.nodes.empty()) return;

        std::unordered_map<int, Vector2> nodeMap;
        for (const auto &node : state.nodes) {
            nodeMap[node.id] = {node.drawX, node.drawY};
        }

        for (const auto& node: state.nodes) {
            if (node.prevNodeIndex != -1 && nodeMap.count(node.prevNodeIndex)) {
                DrawLineEx({node.drawX, node.drawY}, nodeMap[node.prevNodeIndex], 3.0f, DARKGRAY);
            }
        }

        for (const auto& node : state.nodes) {
            DrawCircle(node.drawX, node.drawY, 28, BLACK);
            DrawCircle(node.drawX, node.drawY, 26, node.color);

            if (node.highlightIndex == 1) {
                DrawCircleLines(node.drawX, node.drawY, 22, BLACK);
                DrawCircleLines(node.drawX, node.drawY, 21, BLACK);
            }

            string text;
            int fontSize = 20;
            if (node.data == 0) text = "ROOT", fontSize = 14;
            else text = string(1, (char)node.data), fontSize = 24;

            int textWidth = MeasureText(text.c_str(), fontSize);
            DrawText(text.c_str(), node.drawX - textWidth/2, node.drawY - fontSize/2, fontSize, WHITE);
        }

        DrawText(state.message.c_str(), 20, 20, 25, BLACK);
    }
}