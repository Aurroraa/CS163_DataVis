#pragma once
#include "State.h"
#include "DataStructures/MinHeap.h"

class MinHeapState : public State {
private:
    MinHeap heap;
    // UI State
    bool showCreateMenu = false;
    bool showInsertMenu = false;
    bool showDeleteMenu = false;

    // Input
    bool isInputActive = false;
    char inputBuffer[64] = {0};

    float playbackSpeed = 0.5f;

    // Helpers
    void DrawToolbar();
    void DrawCreateMenu(float x, float y); // <-- NEW
    void DrawInsertMenu(float x, float y);
    void DrawDeleteMenu(float x, float y);

    void DrawPseudocode(); // <-- NEW
    void DrawPlayback();   // <-- NEW
public:
    MinHeapState();

    void Init() override;
    void Update() override;
    void Draw() override;
};