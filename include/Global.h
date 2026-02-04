#pragma once
#include "raylib.h"
#include <string>
#include <vector>
#include "VisualNode.h"

// 1. Constants so we don't use "magic numbers" everywhere
const int SCREEN_WIDTH = 1440;
const int SCREEN_HEIGHT = 900;
const Color PRIMARY_COLOR = { 100, 100, 255, 255 }; // Light Blue
const Color HIGHLIGHT_COLOR = { 255, 161, 0, 255 }; // Orange

enum AppState {
    MENU,           // The Main Home Screen
    SELECT_MODE,    // The "Choose Data Structure" Menu
    ABOUT_US,       // The Credits Screen
    SETTINGS,

    // The Actual Visualizers
    RUNNING_DLL,
    RUNNING_HEAP,
    RUNNING_AVL,
    RUNNING_TRIE,
    RUNNING_MST,
    RUNNING_DIJKSTRA,

    EXIT
};

// 2. The Animation State
// This struct holds EVERYTHING needed to draw the screen at one specific moment.
struct AnimationState {
    // -- Text Info --
    std::string message;        // e.g., "Step 1: Create Node"
    int codeLineIndex;          // Which line of code to highlight (0, 1, 2...)
    
    // -- Visual Data (Generic for now) --
    std::vector<VisualNode> nodes;
    int highlightIndex;         // Which item in the vector is active?
    std::vector<std::string> codeText;
};