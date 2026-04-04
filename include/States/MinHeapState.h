#pragma once
#include "State.h"
#include "../DataStructures/MinHeap.h"
#include "Global.h"
#include "UIHelper.h"

class MinHeapState : public State {
private:
    MinHeap heap;

    // The 5 Main Menu Flags
    bool showInitMenu = false;
    bool showAddMenu = false;
    bool showDeleteMenu = false;
    bool showUpdateMenu = false;
    bool showSearchMenu = false;

    // Inputs
    bool isInputActive = false;
    char inputBuffer[64] = {0};

    bool isLocActive = false;
    char locBuffer[64] = {0};

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

    UIConfig config;
    bool isSettingsOpen = false;
    void DrawSettingsModal();

public:
    MinHeapState();
    void Init() override;
    void Update() override;
    void Draw() override;
};