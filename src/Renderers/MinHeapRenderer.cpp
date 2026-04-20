#include "../../include/Renderers/MinHeapRenderer.h"
#include "../../include/App.h"
#include "raylib.h"
#include <string>
#include "UIHelper.h"

using namespace std;

namespace MinHeapRenderer {
    void Draw(const AnimationState &state, const UIConfig& config) {
        if (state.nodes.empty()) return;

        Color textCol = UIHelper::GetTextCol(config);
        Color nodeBgCol = UIHelper::GetCanvasBg(config);
        Color defaultBorderCol = UIHelper::GetPrimaryCol(config);
        Color lineCol = Fade(textCol, 0.4f);

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

        for (const auto &node : state.nodes) {
            Color currentBorder = node.color;
            if (currentBorder.r == BLUE.r && currentBorder.g == BLUE.g && currentBorder.b == BLUE.b) {
                currentBorder = defaultBorderCol;
            }

            string text = to_string(node.data);
            string indexText = to_string(node.highlightIndex);

            DrawCircle(node.drawX, node.drawY, config.nodeRadius, currentBorder);
            DrawCircle(node.drawX, node.drawY, config.nodeRadius - config.edgeThickness, nodeBgCol);

            Vector2 textSize = MeasureTextEx(g_App->mainFont, text.c_str(), config.textSize, 1.0f);
            DrawTextEx(g_App->mainFont, text.c_str(), {node.drawX - textSize.x/2.0f, node.drawY - textSize.y/2.0f}, config.textSize, 1.0f, textCol);
            
            DrawTextEx(g_App->mainFont, indexText.c_str(), {node.drawX - 5.0f, node.drawY - config.nodeRadius - 20.0f}, 16.0f, 1.0f, currentBorder);

            float boxX = tableStartX + node.highlightIndex * boxSize;

            DrawRectangle(boxX, tableY, boxSize, boxSize, nodeBgCol);
            DrawRectangleLinesEx({boxX, (float)tableY, (float)boxSize, (float)boxSize}, config.edgeThickness, currentBorder);

            Vector2 boxTextWidth = MeasureTextEx(g_App->mainFont, text.c_str(), config.textSize, 1.0f);
            DrawTextEx(g_App->mainFont, text.c_str(), {boxX + boxSize/2.0f - boxTextWidth.x/2.0f, (float)tableY + boxSize/2.0f - boxTextWidth.y/2.0f}, config.textSize, 1.0f, textCol);
            
            DrawTextEx(g_App->mainFont, indexText.c_str(), {boxX + 22.0f, (float)tableY + boxSize + 5.0f}, 16.0f, 1.0f, currentBorder);
        }

        DrawTextEx(g_App->boldFont, state.message.c_str(), {20.0f, 20.0f}, 24.0f, 1.0f, textCol);
    }
}