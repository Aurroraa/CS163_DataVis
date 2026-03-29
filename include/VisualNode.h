#pragma once
#include "raylib.h"
struct VisualNode {
    int id;             // Unique ID to track movement
    int data;
    int highlightIndex;
    float x, y;
    float drawX, drawY;
    Color color;        // Current color (Blue=Normal, Orange=Highlight)

    // For arrow drawing
    int nextNodeIndex;  // -1 if null
    int prevNodeIndex;  // -1 if null
};