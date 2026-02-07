#include "../include/Visualizer.h"
#include "../include/Renderers/DLLRenderer.h"
#include "raylib.h"

// --- Playback Logic ---

void Visualizer::Update() {
    if (isPlaying) {
        timer += GetFrameTime(); // Add time passed since last frame
        if (timer >= playbackSpeed) {
            timer = 0.0f;
            NextStep(); // Auto-advance

            // Stop if we reach the end
            if (currentStep >= history.size() - 1) {
                isPlaying = false;
            }
        }
    }
}

void Visualizer::TogglePlay() {
    isPlaying = !isPlaying;
}

void Visualizer::SetSpeed(float speed) {
    playbackSpeed = speed;
}

void Visualizer::NextStep() {
    if (currentStep < history.size() - 1) currentStep++;
}

void Visualizer::PrevStep() {
    if (currentStep > 0) currentStep--;
}

void Visualizer::GoToStart() {
    currentStep = 0;
    timer = 0;
}

void Visualizer::GoToEnd() {
    if (!history.empty()) currentStep = history.size() - 1;
}

void Visualizer::ClearHistory() {
    history.clear();
    currentStep = 0;
    isPlaying = false;
}

const AnimationState& Visualizer::GetCurrentState() const {
    static AnimationState emptyState;
    if (history.empty()) return emptyState;
    return history[currentStep];
}

// --- Drawing & Recording ---

void Visualizer::DrawCanvas() {
    if (history.empty()) {
        DrawText("List is empty. Create or Insert nodes!", 300, 300, 20, GRAY);
        return;
    }
    const AnimationState& state = history[currentStep];

    // Delegate to Renderer
    DLLRenderer::Draw(state);

    // Draw Step Counter
    DrawText(TextFormat("Step: %d / %d", currentStep + 1, history.size()),
             20, GetScreenHeight() - 230, 20, DARKGRAY);
}

void Visualizer::SetStep(int step) {
    if (step >= 0 && step < history.size()) {
        currentStep = step;
    }
}

void Visualizer::SetPlaying(bool play) {
    isPlaying = play;
}

int Visualizer::GetTotalSteps() const {
    return history.size();
}

void Visualizer::RecordState(std::string msg, int line, std::vector<VisualNode> nodes, std::vector<std::string> code) {
    AnimationState state;
    state.message = msg;
    state.codeLineIndex = line;
    state.nodes = nodes;
    state.codeText = code;
    history.push_back(state);
}