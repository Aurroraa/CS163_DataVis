#include "Renderers/AVLRenderer.h"
#include "raylib.h"
#include <string>
#include <unordered_map>
#include "UIHelper.h"
#include "../../include/App.h" // 🌟 ADDED APP.H

using namespace std;

namespace AVLRenderer {
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
            if (node.nextNodeIndex != -1) {
                DrawLineEx({node.drawX, node.drawY}, nodeMap[node.nextNodeIndex], config.edgeThickness, lineCol);
            }
            if (node.prevNodeIndex != -1) {
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

            // 🌟 PERFECTLY CENTERED NODE TEXT
            string text = to_string(node.data);
            Vector2 textSize = MeasureTextEx(g_App->mainFont, text.c_str(), config.textSize, 1.0f);
            DrawTextEx(g_App->mainFont, text.c_str(), {node.drawX - textSize.x/2.0f, node.drawY - textSize.y/2.0f}, config.textSize, 1.0f, textCol);

            // Balance Factor Text
            string bfText = to_string(node.highlightIndex);
            Color bfColor = abs(node.highlightIndex) > 1 ? RED : currentBorder;
            DrawTextEx(g_App->mainFont, bfText.c_str(), {node.drawX + config.nodeRadius + 5.0f, node.drawY - config.nodeRadius}, 16.0f, 1.0f, bfColor);
        }

        // 🌟 BOLD, 24px STATE MESSAGE
        DrawTextEx(g_App->boldFont, state.message.c_str(), {20.0f, 20.0f}, 24.0f, 1.0f, textCol);
    }
};