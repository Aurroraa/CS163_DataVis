#include "../../include/Renderers/DLLRenderer.h"
#include "raylib.h"
#include <string>

// Helper to draw an arrow between two points
void DrawArrow(Vector2 start, Vector2 end, Color color) {
    // 1. Draw the line
    DrawLineEx(start, end, 2.0f, color);

    // 2. Calculate arrow head (simple triangle)
    // We want the arrow head to be slightly before the center of the node
    // so it doesn't get covered by the circle.
    
    // (Optional: You can add math here to draw a proper triangle arrow head)
    // For now, a small circle at the destination works well for linked lists
    DrawCircleV(end, 3.0f, color); 
}

void DLLRenderer::Draw(const AnimationState& state) {
    // Safety check: If empty, do nothing
    if (state.nodes.empty()) return;

    // ---------------------------------------------------------
    // STEP 1: DRAW ARROWS (Background)
    // ---------------------------------------------------------
    for (const auto& node : state.nodes) {
        // A. Draw "Next" Arrow (Top offset)
        if (node.nextNodeIndex != -1 && node.nextNodeIndex < state.nodes.size()) {
            const auto& target = state.nodes[node.nextNodeIndex];
            
            Vector2 start = {node.x, node.y - 10};   // Start slightly higher
            Vector2 end = {target.x, target.y - 10}; // End slightly higher
            
            DrawArrow(start, end, DARKGRAY);
            
            // Draw small "Next" label (optional)
            // DrawText("next", (start.x + end.x)/2, start.y - 15, 10, GRAY);
        }

        // B. Draw "Prev" Arrow (Bottom offset)
        if (node.prevNodeIndex != -1 && node.prevNodeIndex < state.nodes.size()) {
            const auto& target = state.nodes[node.prevNodeIndex];
            
            Vector2 start = {node.x, node.y + 10};   // Start slightly lower
            Vector2 end = {target.x, target.y + 10}; // End slightly lower
            
            DrawArrow(start, end, DARKGRAY);
        }
    }

    // ---------------------------------------------------------
    // STEP 2: DRAW NODES (Foreground)
    // ---------------------------------------------------------
    for (const auto& node : state.nodes) {
        // 1. Draw Outer Ring (Border)
        DrawCircle(node.x, node.y, 32, BLACK); 

        // 2. Draw Inner Circle (The Node Color)
        // Usually BLUE, but turns ORANGE/RED during animations
        DrawCircle(node.x, node.y, 30, node.color);

        // 3. Draw the Value
        // Center the text based on its length
        std::string text = std::to_string(node.data);
        int textWidth = MeasureText(text.c_str(), 20);
        DrawText(text.c_str(), node.x - textWidth / 2, node.y - 10, 20, WHITE);
        
        // 4. Draw Index/ID (Helpful for debugging)
        DrawText(TextFormat("[%d]", node.id), node.x - 10, node.y + 35, 10, GRAY);
    }

    // ---------------------------------------------------------
    // STEP 3: DRAW STATUS MESSAGE
    // ---------------------------------------------------------
    // Show what the algorithm is currently doing (e.g. "Searching for 42...")
    DrawText(state.message.c_str(), 20, 20, 25, BLACK);
}