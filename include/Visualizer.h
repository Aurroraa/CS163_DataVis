#pragma once
#include "Global.h"
#include <vector>

class Visualizer {
private:
    std::vector<AnimationState> history;
    int currentStep = 0;

    // --- NEW: Playback Control ---
    bool isPlaying = false;
    float playbackSpeed = 0.5f; // Time per step in seconds
    float timer = 0.0f;

    Visualizer() {} // Private Constructor

public:
    static Visualizer& Instance() {
        static Visualizer instance;
        return instance;
    }

    // --- NEW: Update Loop for Auto-Play ---
    void Update();

    void SetStep(int step);
    void SetPlaying(bool play);
    int GetTotalSteps() const;

    // Playback Controls
    void TogglePlay();
    void SetSpeed(float speed); // 0.1 (Fast) to 2.0 (Slow)
    bool IsPlaying() const { return isPlaying; }


    // Navigation
    void NextStep();
    void PrevStep();
    void GoToStart();
    void GoToEnd();

    // Drawing & Logic
    void DrawCanvas();
    void RecordState(std::string msg, int line, std::vector<VisualNode> nodes, std::vector<std::string> code);
    void ClearHistory();

    const AnimationState& GetCurrentState() const;
};