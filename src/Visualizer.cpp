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
    // DLLRenderer::Draw(state);

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

AnimationState Visualizer::GetRenderState() const {
    if (history.empty()) return AnimationState();
    AnimationState renderState = history[currentStep];

    for (auto& node : renderState.nodes) {
        node.drawX = node.x; node.drawY = node.y;
    }

    if (!isPlaying || currentStep >= history.size() - 1) return renderState;

    const AnimationState& nextState = history[currentStep + 1];
    float t = timer / playbackSpeed;
    if (t > 1.0f) t = 1.0f; if (t < 0.0f) t = 0.0f;
    float smooth_t = t * t * (3.0f - 2.0f * t);

    for (auto& currNode : renderState.nodes) {
        for (const auto& nextNode : nextState.nodes) {
            // 🐛 FIX: Match by unique ID so duplicate values don't criss-cross!
            if (currNode.id == nextNode.id) {
                currNode.drawX = currNode.x + (nextNode.x - currNode.x) * smooth_t;
                currNode.drawY = currNode.y + (nextNode.y - currNode.y) * smooth_t;
                break;
            }
        }
    }
    return renderState;
}