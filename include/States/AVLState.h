#pragma once
#include "State.h"
#include "../DataStructures/AVLTree.h"

class AVLState : public State {
private:
    AVLTree avl;
    float playbackSpeed = 0.5f;

    // UI flags (same as Heap)
    bool showCreateMenu = false;
    bool showInsertMenu = false;
    bool showDeleteMenu = false;

    char inputBuffer[64] = {0};
    bool isInputActive = false;

    void DrawToolbar();
    void DrawCreateMenu(float x, float y); // <-- NEW
    void DrawInsertMenu(float x, float y);
    void DrawDeleteMenu(float x, float y);

    void DrawPseudocode(); // <-- NEW
    void DrawPlayback();   // <-- NEW

public:
    AVLState();

    void Init() override;
    void Update() override;
    void Draw() override;
};