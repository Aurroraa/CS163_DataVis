#pragma once
#include "State.h"

class MenuState : public State {
public:
    void Init() override;
    void Update() override;
    void Draw() override;
};