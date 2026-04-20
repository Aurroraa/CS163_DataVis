#include "../../include/Renderers/TrieRenderer.h"
#include "../../include/App.h"
#include "raylib.h"
#include <string>
#include <unordered_map>
#include "UIHelper.h"
using namespace std;

namespace TrieRenderer {
    void Draw(const AnimationState& state, const UIConfig& config) {
        if (state.nodes.empty()) return;

        Color textCol = UIHelper::GetTextCol(config);
        Color nodeBgCol = UIHelper::GetCanvasBg(config);
        Color defaultBorderCol = UIHelper::GetPrimaryCol(config);
        Color lineCol = Fade(textCol, 0.4f);

        std::unordered_map<int, Vector2> nodeMap;
        for (const auto &node : state.nodes) {
            nodeMap[node.id] = {node.drawX, node.drawY};
        }

        for (const auto& node: state.nodes) {
            if (node.prevNodeIndex != -1 && nodeMap.count(node.prevNodeIndex)) {
                DrawLineEx({node.drawX, node.drawY}, nodeMap[node.prevNodeIndex], config.edgeThickness, lineCol);
            }
        }

        for (const auto& node : state.nodes) {
            Color currentBorder = node.color;
            if (currentBorder.r == BLUE.r && currentBorder.g == BLUE.g && currentBorder.b == BLUE.b) {
                currentBorder = defaultBorderCol;
            }

            DrawCircle(node.drawX, node.drawY, config.nodeRadius, currentBorder);
            DrawCircle(node.drawX, node.drawY, config.nodeRadius - config.edgeThickness, nodeBgCol);

            if (node.highlightIndex == 1) {
                DrawCircleLines(node.drawX, node.drawY, config.nodeRadius - config.edgeThickness - 4, currentBorder);
                DrawCircleLines(node.drawX, node.drawY, config.nodeRadius - config.edgeThickness - 5, currentBorder);
            }

            string text;
            float fontSize = config.textSize;
            if (node.data == 0) {
                text = "ROOT";
                fontSize = config.textSize * 0.6f; 
            } else {
                text = string(1, (char)node.data);
            }

            Vector2 textSize = MeasureTextEx(g_App->mainFont, text.c_str(), fontSize, 1.0f);
            DrawTextEx(g_App->mainFont, text.c_str(), {node.drawX - textSize.x/2.0f, node.drawY - textSize.y/2.0f}, fontSize, 1.0f, textCol);
        }

        DrawTextEx(g_App->boldFont, state.message.c_str(), {20.0f, 20.0f}, 24.0f, 1.0f, textCol);
    }
}