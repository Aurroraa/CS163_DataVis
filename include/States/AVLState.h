#pragma once
#include "State.h"
#include "../DataStructures/AVLTree.h"

class AVLState : public State {
private:
    AVLTree avl;

    // The 5 Main Menu Flags
    bool showInitMenu = false;
    bool showAddMenu = false;
    bool showDeleteMenu = false;
    bool showUpdateMenu = false;
    bool showSearchMenu = false;

    // Inputs
    bool isInputActive = false;
    char inputBuffer[64] = {0}; // Used for Value / New Value

    bool isOldActive = false;
    char oldBuffer[64] = {0};   // Used for Old Value in Update

    bool nInputActive = false;
    char nBuffer[16] = "5";

    float playbackSpeed = 0.5f;

    // The Bottom Toolbar
    void DrawToolbar();

    // Flattened Popup Screens
    void DrawInitMenu(float x, float y);
    void DrawAddMenu(float x, float y);
    void DrawDeleteMenu(float x, float y);
    void DrawUpdateMenu(float x, float y);
    void DrawSearchMenu(float x, float y);

    void DrawPseudocode();
    void DrawPlayback();

public:
    AVLState();
    void Init() override;
    void Update() override;
    void Draw() override;
};