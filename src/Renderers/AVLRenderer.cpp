#include "Renderers/AVLRenderer.h"
#include "raylib.h"
#include <string>
#include <unordered_map>
using namespace std;

namespace AVLRenderer {
    void Draw(const AnimationState& state) {
        if (state.nodes.empty()) return;

        std::unordered_map<int, Vector2> nodeMap;
        for (const auto &node : state.nodes) {
            nodeMap[node.id] = {node.x, node.y};
        }

        for (const auto& node: state.nodes) {
            if (node.nextNodeIndex != -1) {
                DrawLineEx({node.x, node.y}, nodeMap[node.nextNodeIndex], 3.0f, DARKGRAY);
            }
            if (node.prevNodeIndex != -1) {
                DrawLineEx({node.x, node.y}, nodeMap[node.prevNodeIndex], 3.0f, DARKGRAY);
            }
        }

        for (const auto& node : state.nodes) {
            string text = to_string(node.data);
            string bfText = to_string(node.highlightIndex);

            DrawCircle(node.x, node.y, 28, BLACK);
            DrawCircle(node.x, node.y, 26, node.color);

            int textWidth = MeasureText(text.c_str(), 20);
            DrawText(text.c_str(), node.x - textWidth/2, node.y - 10, 20, WHITE);

            Color bfColor = abs(node.highlightIndex) > 1 ? RED : DARKBLUE;
            DrawText(bfText.c_str(), node.x + 20, node.y - 30, 15, bfColor);
        }

        DrawText(state.message.c_str(), 20, 20, 25, BLACK);
    }
};