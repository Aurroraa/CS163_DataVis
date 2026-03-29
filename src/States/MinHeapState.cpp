#include "../../include/States/MinHeapState.h"
#include "../../include/States/SelectState.h"
#include "../../include/App.h"
#include "../../include/raygui.h"
#include "../../include/Visualizer.h"
#include "../../include/tinyfiledialogs.h"
#include "../../include/Renderers/MinHeapRenderer.h"
#include <cstdlib>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>

MinHeapState::MinHeapState() {
    Visualizer::Instance().ClearHistory();
    heap.init({10, 20, 15, 40, 50, 100, 25});
    playbackSpeed = 0.5f;
}

void MinHeapState::Init() {
    showInitMenu = false;
    showAddMenu = false;
    showDeleteMenu = false;
    showUpdateMenu = false;
    showSearchMenu = false;
    inputBuffer[0] = '\0';
    locBuffer[0] = '\0';
    Visualizer::Instance().SetSpeed(playbackSpeed);
}

void MinHeapState::Update() {
    Visualizer::Instance().Update();
}

void MinHeapState::Draw() {
    // 1. Canvas restored to float above the toolbar
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight() - 200, RAYWHITE);
    Visualizer::Instance().DrawCanvas();

    // 2. Navigation
    if (GuiButton((Rectangle){(float)GetScreenWidth() - 100, 10, 80, 30}, "Home")) {
        g_App->ChangeState(new SelectState());
    }

    // 3. Draw Data Structure (Using the smooth animation state!)
    MinHeapRenderer::Draw(Visualizer::Instance().GetRenderState());

    // 4. UI Components
    DrawToolbar();
    DrawPlayback();
    DrawPseudocode();
}

void MinHeapState::DrawToolbar() {
    float startY = GetScreenHeight() - 200;

    DrawRectangle(0, startY, GetScreenWidth(), 200, LIGHTGRAY);
    DrawLine(0, startY, GetScreenWidth(), startY, DARKGRAY);

    int btnWidth = 100;
    int gap = 10;
    float x = 20;

    #define CLOSE_ALL_MENUS() showInitMenu = showAddMenu = showDeleteMenu = showUpdateMenu = showSearchMenu = false;

    // --- THE MAIN BUTTONS ---
    if (GuiButton((Rectangle){x, startY + 10, (float)btnWidth, 40}, "Init")) {
        bool wasOpen = showInitMenu; CLOSE_ALL_MENUS(); showInitMenu = !wasOpen;
    } x += btnWidth + gap;

    if (GuiButton((Rectangle){x, startY + 10, (float)btnWidth, 40}, "Add")) {
        bool wasOpen = showAddMenu; CLOSE_ALL_MENUS(); showAddMenu = !wasOpen;
    } x += btnWidth + gap;

    if (GuiButton((Rectangle){x, startY + 10, (float)btnWidth, 40}, "Delete")) {
        bool wasOpen = showDeleteMenu; CLOSE_ALL_MENUS(); showDeleteMenu = !wasOpen;
    } x += btnWidth + gap;

    if (GuiButton((Rectangle){x, startY + 10, (float)btnWidth, 40}, "Update")) {
        bool wasOpen = showUpdateMenu; CLOSE_ALL_MENUS(); showUpdateMenu = !wasOpen;
    } x += btnWidth + gap;

    if (GuiButton((Rectangle){x, startY + 10, (float)btnWidth, 40}, "Search")) {
        bool wasOpen = showSearchMenu; CLOSE_ALL_MENUS(); showSearchMenu = !wasOpen;
    } x += btnWidth + gap + 20;

    // --- THE POPUPS ---
    if (showInitMenu)   DrawInitMenu(20, startY + 60);
    if (showAddMenu)    DrawAddMenu(20, startY + 60);
    if (showDeleteMenu) DrawDeleteMenu(20, startY + 60);
    if (showUpdateMenu) DrawUpdateMenu(20, startY + 60);
    if (showSearchMenu) DrawSearchMenu(20, startY + 60);
}

// ---------------------------------------------------------
// FLATTENED POPUP SCREENS
// ---------------------------------------------------------
void MinHeapState::DrawInitMenu(float x, float y) {
    DrawRectangle(x, y, 620, 120, RAYWHITE);
    DrawRectangleLines(x, y, 620, 120, GRAY);

    // Row 1
    if (GuiButton((Rectangle){x + 10, y + 10, 120, 30}, "Empty (Clear)")) {
        Visualizer::Instance().ClearHistory();
        heap.init({});
        Visualizer::Instance().SetPlaying(false);
    }

    DrawText("A =", x + 150, y + 15, 20, BLACK);
    if (GuiTextBox((Rectangle){x + 190, y + 10, 150, 30}, inputBuffer, 64, isInputActive)) isInputActive = !isInputActive;
    if (GuiButton((Rectangle){x + 350, y + 10, 40, 30}, "Go") || (IsKeyPressed(KEY_ENTER))) {
        Visualizer::Instance().ClearHistory();
        std::vector<int> values;
        std::stringstream ss(inputBuffer);
        std::string segment;
        while(std::getline(ss, segment, ',')) { try { values.push_back(std::stoi(segment)); } catch(...) {} }
        if (!values.empty()) heap.init(values);
        Visualizer::Instance().SetStep(0);
        Visualizer::Instance().SetPlaying(true);

        inputBuffer[0] = '\0';
        isInputActive = false;
    }

    // Row 2
    DrawText("Random N =", x + 10, y + 65, 20, BLACK);
    if (GuiTextBox((Rectangle){x + 130, y + 60, 50, 30}, nBuffer, 16, nInputActive)) nInputActive = !nInputActive;
    if (GuiButton((Rectangle){x + 190, y + 60, 90, 30}, "Generate") || (IsKeyPressed(KEY_ENTER))) {
        Visualizer::Instance().ClearHistory();
        int n = atoi(nBuffer);
        if (n < 1) n = 1; if (n > 31) n = 31;
        std::vector<int> values;
        for(int i = 0; i < n; i++) values.push_back(GetRandomValue(1, 99));
        heap.init(values);
        Visualizer::Instance().SetStep(0);
        Visualizer::Instance().SetPlaying(true);

        nBuffer[0] = '\0';
        nInputActive = false;
    }

    if (GuiButton((Rectangle){x + 300, y + 60, 120, 30}, "Browse File...")) {
        const char *filterPatterns[1] = { "*.txt" };
        const char *filePath = tinyfd_openFileDialog("Select Input File", "", 1, filterPatterns, "Text Files", 0);
        if (filePath != NULL) {
            std::ifstream file(filePath);
            if (file.is_open()) {
                std::vector<int> values; int val;
                while (file >> val) { values.push_back(val); if (file.peek() == ',') file.ignore(); }
                file.close();
                if (!values.empty()) heap.init(values);
                Visualizer::Instance().SetStep(0);
                Visualizer::Instance().SetPlaying(true);
            }
        }
    }
}

void MinHeapState::DrawAddMenu(float x, float y) {
    DrawRectangle(x, y, 200, 120, RAYWHITE);
    DrawRectangleLines(x, y, 200, 120, GRAY);

    DrawText("Value:", x + 10, y + 25, 20, BLACK);
    if (GuiTextBox((Rectangle){x + 80, y + 20, 80, 30}, inputBuffer, 64, isInputActive)) isInputActive = !isInputActive;

    if (GuiButton((Rectangle){x + 10, y + 70, 150, 30}, "Insert Value") || (IsKeyPressed(KEY_ENTER))) {
        Visualizer::Instance().ClearHistory();
        Visualizer::Instance().RecordState("Initial State", 0, heap.captureState(), {});
        heap.insert(atoi(inputBuffer));
        Visualizer::Instance().SetStep(0);
        Visualizer::Instance().SetPlaying(true);
        inputBuffer[0] = '\0';
        isInputActive = false;
    }
}

void MinHeapState::DrawDeleteMenu(float x, float y) {
    DrawRectangle(x, y, 400, 120, RAYWHITE);
    DrawRectangleLines(x, y, 400, 120, GRAY);

    // Button 1: Extract Min (No input needed)
    if (GuiButton((Rectangle){x + 10, y + 20, 150, 30}, "Extract Min")) {
        Visualizer::Instance().ClearHistory();
        Visualizer::Instance().RecordState("Initial State", 0, heap.captureState(), {});
        heap.extractMin();
        Visualizer::Instance().SetStep(0);
        Visualizer::Instance().SetPlaying(true);
    }

    // Button 2: Delete at Position
    DrawText("Pos:", x + 180, y + 25, 20, BLACK);
    if (GuiTextBox((Rectangle){x + 230, y + 20, 60, 30}, locBuffer, 64, isLocActive)) isLocActive = !isLocActive;

    if (GuiButton((Rectangle){x + 180, y + 70, 150, 30}, "Delete at Pos") || (IsKeyPressed(KEY_ENTER))) {
        Visualizer::Instance().ClearHistory();
        Visualizer::Instance().RecordState("Initial State", 0, heap.captureState(), {});
        heap.deleteNode(atoi(locBuffer));
        Visualizer::Instance().SetStep(0);
        Visualizer::Instance().SetPlaying(true);

        locBuffer[0] = '\0';
        isLocActive = false;
    }
}

void MinHeapState::DrawUpdateMenu(float x, float y) {
    DrawRectangle(x, y, 350, 120, RAYWHITE);
    DrawRectangleLines(x, y, 350, 120, GRAY);

    DrawText("Loc:", x + 10, y + 25, 20, BLACK);
    if (GuiTextBox((Rectangle){x + 60, y + 20, 60, 30}, locBuffer, 64, isLocActive)) isLocActive = !isLocActive;

    DrawText("New Val:", x + 140, y + 25, 20, BLACK);
    if (GuiTextBox((Rectangle){x + 230, y + 20, 80, 30}, inputBuffer, 64, isInputActive)) isInputActive = !isInputActive;

    if (GuiButton((Rectangle){x + 10, y + 70, 200, 30}, "Update Location")) {
        Visualizer::Instance().ClearHistory();
        Visualizer::Instance().RecordState("Initial State", 0, heap.captureState(), {});
        heap.updateNode(atoi(locBuffer), atoi(inputBuffer));
        Visualizer::Instance().SetStep(0);
        Visualizer::Instance().SetPlaying(true);

        inputBuffer[0] = '\0';
        isInputActive = false;
        locBuffer[0] = '\0';
        isLocActive = false;
    }
}

void MinHeapState::DrawSearchMenu(float x, float y) {
    DrawRectangle(x, y, 350, 120, RAYWHITE);
    DrawRectangleLines(x, y, 350, 120, GRAY);

    // Search by Value
    DrawText("Val:", x + 10, y + 25, 20, BLACK);
    if (GuiTextBox((Rectangle){x + 50, y + 20, 60, 30}, inputBuffer, 64, isInputActive)) isInputActive = !isInputActive;
    if (GuiButton((Rectangle){x + 10, y + 70, 100, 30}, "Search Val") || (IsKeyPressed(KEY_ENTER))) {
        Visualizer::Instance().ClearHistory();
        Visualizer::Instance().RecordState("Initial State", 0, heap.captureState(), {});
        heap.searchNode(atoi(inputBuffer));
        Visualizer::Instance().SetStep(0);
        Visualizer::Instance().SetPlaying(true);
        inputBuffer[0] = '\0';
        isInputActive = false;
    }

    // Search by Position
    DrawText("Pos:", x + 140, y + 25, 20, BLACK);
    if (GuiTextBox((Rectangle){x + 190, y + 20, 60, 30}, locBuffer, 64, isLocActive)) isLocActive = !isLocActive;
    if (GuiButton((Rectangle){x + 140, y + 70, 110, 30}, "Search Pos") || (IsKeyPressed(KEY_ENTER))) {
        Visualizer::Instance().ClearHistory();
        Visualizer::Instance().RecordState("Initial State", 0, heap.captureState(), {});
        heap.searchPosition(atoi(locBuffer));
        Visualizer::Instance().SetStep(0);
        Visualizer::Instance().SetPlaying(true);
        locBuffer[0] = '\0';
        isLocActive = false;
    }
}

void MinHeapState::DrawPlayback() {
    int centerX = GetScreenWidth() / 2;
    int y = GetScreenHeight() - 240; // Floating perfectly above the bottom toolbar

    if (GuiButton((Rectangle){(float)centerX - 100, (float)y, 40, 30}, "<<")) Visualizer::Instance().GoToStart();
    if (GuiButton((Rectangle){(float)centerX - 50, (float)y, 40, 30}, "<")) Visualizer::Instance().PrevStep();

    const char* label = Visualizer::Instance().IsPlaying() ? "||" : ">";
    if (GuiButton((Rectangle){(float)centerX, (float)y, 40, 30}, label)) Visualizer::Instance().TogglePlay();

    if (GuiButton((Rectangle){(float)centerX + 50, (float)y, 40, 30}, ">")) Visualizer::Instance().NextStep();
    if (GuiButton((Rectangle){(float)centerX + 100, (float)y, 40, 30}, ">>")) Visualizer::Instance().GoToEnd();

    DrawText("Speed:", centerX + 220, y + 5, 20, BLACK);
    GuiSlider((Rectangle){(float)centerX + 350, (float)y, 100, 30}, "Fast", "Slow", &playbackSpeed, 0.1f, 2.0f);
    Visualizer::Instance().SetSpeed(playbackSpeed);
}

void MinHeapState::DrawPseudocode() {
    int panelW = 600;
    int panelH = 200;
    int startX = GetScreenWidth() - panelW;
    int startY = GetScreenHeight() - 200;

    // Background
    DrawRectangle(startX, startY, panelW, panelH, Fade(BLACK, 0.8f));
    DrawRectangleLines(startX, startY, panelW, panelH, GRAY);
    DrawText("Pseudocode", startX + 10, startY + 10, 20, WHITE);

    // Get Data
    const AnimationState& state = Visualizer::Instance().GetCurrentState();
    if (state.codeText.empty()) return;

    // Draw Lines
    int y = startY + 40;
    for (int i = 0; i < state.codeText.size(); i++) {
        if (i == state.codeLineIndex) {
            DrawRectangle(startX, y - 2, panelW, 25, BLACK);
            DrawText(state.codeText[i].c_str(), startX + 10, y, 20, YELLOW);
        } else {
            DrawText(state.codeText[i].c_str(), startX + 10, y, 20, LIGHTGRAY);
        }
        y += 25;
    }
}