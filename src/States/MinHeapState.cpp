#include "States/MinHeapState.h"
#include "Renderers/MinHeapRenderer.h"
#include "Visualizer.h"
#include "App.h"
#include "raygui.h"

void MinHeapState::Init() {
    Visualizer::Instance().ClearHistory();
    heap.init({10, 20, 15, 40, 50, 100, 25});
}

void MinHeapState::Update() {
    Visualizer::Instance().Update();
}

void MinHeapState::Draw() {
    Visualizer::Instance().DrawCanvas();
    MinHeapRenderer::Draw(Visualizer::Instance().GetCurrentState());
    DrawToolBar();
}

void MinHeapState::DrawToolBar() {
    int screenH = GetScreenHeight();

    if (GuiButton((Rectangle){20, 60, 80, 30}, "Back")) {
        Visualizer::Instance().PrevStep();
    }
    if (GuiButton((Rectangle){20, (float)screenH - 60, 60, 30}, "< Prev")) {
        Visualizer::Instance().PrevStep();
    }
    if (GuiButton((Rectangle){90, (float)screenH - 60, 60, 30}, Visualizer::Instance().IsPlaying() ? "Pause" : "Play")) {
        Visualizer::Instance().TogglePlay();
    }
    if (GuiButton((Rectangle){160, (float)screenH - 60, 60, 30}, "Next >")) {
        Visualizer::Instance().NextStep();
    }

    if (GuiButton((Rectangle){300, (float)screenH - 60, 120, 30}, "Insert Random")) {
        int startStep = Visualizer::Instance().GetTotalSteps();

        heap.insert(GetRandomValue(1, 99));

        Visualizer::Instance().SetStep(startStep);
        Visualizer::Instance().SetPlaying(true);
    }

    if (GuiButton((Rectangle){430, (float)screenH - 60, 120, 30}, "Extract Min")) {
        int startStep = Visualizer::Instance().GetTotalSteps();

        heap.extractMin();

        Visualizer::Instance().SetStep(startStep);
        Visualizer::Instance().SetPlaying(true);
    }

    if (GuiButton((Rectangle){560, (float)screenH - 60, 80, 30}, "Clear")) {
        Visualizer::Instance().ClearHistory();

        heap.init({});

        Visualizer::Instance().SetStep(0);
        Visualizer::Instance().SetPlaying(false);
    }
}
