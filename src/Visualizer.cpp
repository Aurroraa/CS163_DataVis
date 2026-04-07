#include "../include/Visualizer.h"
#include "../include/Renderers/DLLRenderer.h"
#include "raylib.h"
#include "App.h"

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
        const char* emptyMsg = "Data Structure is empty. Initialize or Insert nodes!";
        float fontSize = 24.0f;
        Vector2 msgSize = MeasureTextEx(g_App->mainFont, emptyMsg, fontSize, 1.0f);

        // 🌟 Perfectly center the message in the canvas area (above the 200px toolbar)
        float cx = GetScreenWidth() / 2.0f - msgSize.x / 2.0f;
        float cy = (GetScreenHeight() - 200.0f) / 2.0f - msgSize.y / 2.0f;

        DrawTextEx(g_App->mainFont, emptyMsg, {cx, cy}, fontSize, 1.0f, GRAY);
        return;
    }

    const AnimationState& state = history[currentStep];

    // Delegate to Renderer
    // DLLRenderer::Draw(state);

    // 🌟 Draw Step Counter using the bold custom font
    const char* stepText = TextFormat("Step: %d / %d", currentStep + 1, (int)history.size());
    DrawTextEx(g_App->boldFont, stepText, {20.0f, (float)GetScreenHeight() - 235.0f}, 24.0f, 1.0f, DARKGRAY);
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

void Visualizer::RecordState(std::string message, int codeLineIndex, AnimationState state, std::vector<std::string> codeText) {
    state.message = message;
    state.codeLineIndex = codeLineIndex;
    state.codeText = codeText;

    // Change these two lines to match your exact variable names!
    history.push_back(state);
}