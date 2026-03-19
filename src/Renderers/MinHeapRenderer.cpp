#include "Renderers/MinHeapRenderer.h"
#include "raylib.h"
#include <string>

using namespace std;

void MinHeapRenderer::Draw(const AnimationState &state) {
    if (state.nodes.empty()) return;

    for (const auto &node : state.nodes) {
        if (node.nextNodeIndex != -1) {
            const auto &target = state.nodes[node.nextNodeIndex];
            DrawLineEx({node.x, node.y}, {target.x, target.y}, 3.0f, DARKGRAY);
        }
        if (node.prevNodeIndex != -1) {
            const auto &target = state.nodes[node.prevNodeIndex];
            DrawLineEx({node.x, node.y}, {target.x, target.y}, 3.0f, DARKGRAY);
        }
    }

    int boxSize = 55;
    int tableStartX = (GetScreenWidth() - (state.nodes.size() * boxSize)) / 2;
    int tableY = GetScreenHeight() - 150;

    for (const auto &node : state.nodes) {
        string text = to_string(node.data);
        string indexText = to_string(node.id);

        //Draw TREE
        DrawCircle(node.x, node.y, 28, BLACK);
        DrawCircle(node.x, node.y, 26, node.color);

        int textWidth = MeasureText(text.c_str(), 20);
        DrawText(text.c_str(), node.x - textWidth/2, node.y - 10, 20, WHITE);

        DrawText(indexText.c_str(), node.x - 5, node.y - 45, 15, RED);

        //Draw ARRAY

        int boxX = tableStartX + node.id * boxSize;

        DrawRectangle(boxX, tableY, boxSize, boxSize, node.color);
        DrawRectangleLines(boxX, tableY, boxSize, boxSize, BLACK);

        int boxTextWidth = MeasureText(text.c_str(), 20);
        DrawText(text.c_str(), boxX + boxSize/2 - boxTextWidth/2, tableY + 18, 20, WHITE);

        DrawText(indexText.c_str(), boxX + 22, tableY + boxSize + 5, 15, RED);
    }
}
