#pragma once
#include "State.h"
#include "../DataStructures/Graph.h"

class GraphState : public State {
private:
    Graph graph;

    int draggingVertex;
    bool isDirected = false;
    bool isAlgorithmActive = false;
    bool isDijkstraSourceActive = false;
    char dijkstraSourceBuffer[16] = {0};

    // Inputs
    bool isEditorActive = false;
    char textEditorBuffer[2048] = {0};
    std::string lastText = "";

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
    GraphState();
    void Init() override;
    void Update() override;
    void Draw() override;
};