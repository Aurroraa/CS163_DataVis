#pragma once
#include "State.h"
#include "DataStructures/MinHeap.h"

class MinHeapState : public State {
private:
    MinHeap heap;
    bool isInputActive = false;
    char inputBuffer[64] = {0};

    void DrawToolBar();
public:
    MinHeapState() = default;
    ~MinHeapState() = default;

    void Init() override;
    void Update() override;
    void Draw() override;
};