#pragma once
#include "State.h"

class SelectState : public State {
public:
    void Init() override;
    void Update() override;
    void Draw() override;
};