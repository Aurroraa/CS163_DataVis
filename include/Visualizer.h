#pragma once
#include "Global.h"
#include <vector>
#include "VisualNode.h"

class Visualizer {
private:
    // --- The Core Engine Variables ---
    AppState currentState;
    std::vector<AnimationState> history; // The "Film Roll"
    int currentStep;                     // Current frame being shown (0 to history.size()-1)
    bool isPlaying;                      // Auto-play toggle
    float playbackSpeed;                 // Time between steps
    std::vector<std::string> currentCodeBlock;
    // --- Private Helpers ---
    void handleInput();                  // Listen for keyboard/mouse
    void drawControls();                 // Draw the Next/Prev buttons
    void drawCodeBox(const AnimationState& state);    // Draw the source code panel
    void drawMainMenu();
    void drawSelectMode(); // New: Grid of buttons
    void drawAboutUs();    // New: Text info
    void drawSettings();

public:
    Visualizer(); // Constructor
    
    // 1. Setup
    void init();
    
    // 2. Main Loop
    void run();
    
    // 3. Recorder
    // Call this function from your algorithms to save a snapshot
    void recordState(std::string msg, int line, std::vector<VisualNode> data);
    void setCodeLines(std::vector<std::string> lines);
};