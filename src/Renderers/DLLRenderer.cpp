#include "../../include/Renderers/DLLRenderer.h"
#include "../../include/App.h"
#include "raylib.h"
#include <string>
#include <cmath>

namespace DLLRenderer {

    void DrawFilledArrowHead(Vector2 start, Vector2 end, Color color) {
        float arrowLen = 15.0f; 
        float arrowWide = 6.0f; 

        float dx = end.x - start.x;
        float dy = end.y - start.y;
        float len = sqrtf(dx*dx + dy*dy);
        if (len == 0) return;

        dx /= len; dy /= len;

        Vector2 base = { end.x - dx*arrowLen, end.y - dy*arrowLen };
        Vector2 perp = { -dy * arrowWide, dx * arrowWide };

        Vector2 p1 = { base.x + perp.x, base.y + perp.y };
        Vector2 p2 = { base.x - perp.x, base.y - perp.y };

        DrawTriangle(end, p1, p2, color); 
        DrawTriangle(end, p2, p1, color); 
    }

    void DrawArrowStyle(Vector2 start, Vector2 end, Color color, bool isPrev) {
        float nodeRadius = 30.0f;
        float offset = isPrev ? -7.0f : 7.0f; 

        bool isVertical = (fabs(start.y - end.y) > 10.0f);
        Vector2 pStart, pEnd;

        if (!isVertical) {
            pStart.y = start.y + offset;
            pEnd.y   = end.y + offset;
            if (end.x > start.x) { 
                pStart.x = start.x + nodeRadius;
                pEnd.x   = end.x - nodeRadius;
            } else { 
                pStart.x = start.x - nodeRadius;
                pEnd.x   = end.x + nodeRadius;
            }
        } else {
            pStart.x = start.x + offset;
            pEnd.x   = end.x + offset;
            if (end.y > start.y) { 
                pStart.y = start.y + nodeRadius;
                pEnd.y   = end.y - nodeRadius;
            } else { 
                pStart.y = start.y - nodeRadius;
                pEnd.y   = end.y + nodeRadius;
            }
        }

        DrawLineEx(pStart, pEnd, 2.5f, color);
        DrawFilledArrowHead(pStart, pEnd, color);
    }

    void Draw(const AnimationState& state, const UIConfig& config) {
        if (state.nodes.empty()) return;

        Color textCol = config.isDarkMode ? Color{226, 215, 193, 255} : Color{40, 40, 40, 255};
        Color nodeBgCol = config.isDarkMode ? Color{35, 41, 49, 255} : Color{250, 250, 250, 255};
        Color defaultBorderCol = config.isDarkMode ? Color{162, 151, 137, 255} : Color{242, 182, 182, 255};

        for (const auto& node : state.nodes) {
            if (node.nextNodeIndex != -1 && node.nextNodeIndex < state.nodes.size()) {
                const auto& target = state.nodes[node.nextNodeIndex];
                DrawArrowStyle({node.x, node.y}, {target.x, target.y}, textCol, false);
            }
            if (node.prevNodeIndex != -1 && node.prevNodeIndex < state.nodes.size()) {
                const auto& target = state.nodes[node.prevNodeIndex];
                DrawArrowStyle({node.x, node.y}, {target.x, target.y}, Fade(textCol, 0.3f), true);
            }
        }

        for (const auto& node : state.nodes) {
            Color currentBorder = node.color;
            if (currentBorder.r == BLUE.r && currentBorder.g == BLUE.g && currentBorder.b == BLUE.b) {
                currentBorder = defaultBorderCol;
            }

            DrawCircle(node.drawX, node.drawY, config.nodeRadius, currentBorder);
            DrawCircle(node.drawX, node.drawY, config.nodeRadius - config.edgeThickness, nodeBgCol);

            std::string text = std::to_string(node.data);
            Vector2 textSize = MeasureTextEx(g_App->mainFont, text.c_str(), config.textSize, 1.0f);
            DrawTextEx(g_App->mainFont, text.c_str(), {node.drawX - textSize.x / 2.0f, node.drawY - textSize.y / 2.0f}, config.textSize, 1.0f, textCol);

            const char* idxText = TextFormat("%d", node.id);
            Vector2 idxSize = MeasureTextEx(g_App->mainFont, idxText, 16.0f, 1.0f);
            DrawTextEx(g_App->mainFont, idxText, {node.drawX - idxSize.x / 2.0f, node.drawY + config.nodeRadius + 5.0f}, 16.0f, 1.0f, currentBorder);
        }

        DrawTextEx(g_App->boldFont, state.message.c_str(), {20.0f, 20.0f}, 24.0f, 1.0f, textCol);
    }
}