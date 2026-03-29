#include "../../include/States/AVLState.h"
#include "../../include/States/SelectState.h"
#include "../../include/App.h"
#include "../../include/raygui.h"
#include "../../include/Visualizer.h"
#include "../../include/tinyfiledialogs.h"
#include "../../include/Renderers/AVLRenderer.h"
#include <cstdlib>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>

AVLState::AVLState() {
    Visualizer::Instance().ClearHistory();
    avl.init({});
    playbackSpeed = 0.5f;
}

void AVLState::Init() {
    showInitMenu = false;
    showAddMenu = false;
    showDeleteMenu = false;
    showUpdateMenu = false;
    showSearchMenu = false;
    inputBuffer[0] = '\0';
    oldBuffer[0] = '\0';
    Visualizer::Instance().SetSpeed(playbackSpeed);
}

void AVLState::Update() {
    Visualizer::Instance().Update();
}

void AVLState::Draw() {
    // 1. Canvas restored to float above the toolbar
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight() - 200, RAYWHITE);
    Visualizer::Instance().DrawCanvas();

    // 2. Navigation
    if (GuiButton((Rectangle){(float)GetScreenWidth() - 100, 10, 80, 30}, "Home")) {
        g_App->ChangeState(new SelectState());
    }

    // 3. Draw Data Structure (Using the smooth animation state!)
    AVLRenderer::Draw(Visualizer::Instance().GetRenderState());

    // 4. UI Components
    DrawToolbar();
    DrawPlayback();
    DrawPseudocode();
}

void AVLState::DrawToolbar() {
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
// FLATTENED POPUP SCREENS (With Enter Key & Auto-Clear)
// ---------------------------------------------------------
void AVLState::DrawInitMenu(float x, float y) {
    DrawRectangle(x, y, 620, 120, RAYWHITE);
    DrawRectangleLines(x, y, 620, 120, GRAY);

    // Row 1
    if (GuiButton((Rectangle){x + 10, y + 10, 120, 30}, "Empty (Clear)")) {
        Visualizer::Instance().ClearHistory();
        avl.init({});
        Visualizer::Instance().SetPlaying(false);
    }

    DrawText("A =", x + 150, y + 15, 20, BLACK);
    if (GuiTextBox((Rectangle){x + 190, y + 10, 150, 30}, inputBuffer, 64, isInputActive)) isInputActive = !isInputActive;

    // Trigger on Button OR Enter Key
    if (GuiButton((Rectangle){x + 350, y + 10, 40, 30}, "Go") || (IsKeyPressed(KEY_ENTER))) {
        if (inputBuffer[0] != '\0') {
            Visualizer::Instance().ClearHistory();
            std::vector<int> values;
            std::stringstream ss(inputBuffer);
            std::string segment;
            while(std::getline(ss, segment, ',')) { try { values.push_back(std::stoi(segment)); } catch(...) {} }
            if (!values.empty()) avl.init(values);

            Visualizer::Instance().SetStep(0);
            Visualizer::Instance().SetPlaying(true);

            // Clean up UI
            inputBuffer[0] = '\0';
            isInputActive = false;
        }
    }

    // Row 2
    DrawText("Random N =", x + 10, y + 65, 20, BLACK);
    if (GuiTextBox((Rectangle){x + 130, y + 60, 50, 30}, nBuffer, 16, nInputActive)) nInputActive = !nInputActive;

    // Trigger on Button OR Enter Key
    if (GuiButton((Rectangle){x + 190, y + 60, 90, 30}, "Generate") || (IsKeyPressed(KEY_ENTER))) {
        Visualizer::Instance().ClearHistory();
        int n = atoi(nBuffer);
        if (n < 1) n = 1; if (n > 31) n = 31;
        std::vector<int> values;
        for(int i = 0; i < n; i++) values.push_back(GetRandomValue(1, 99));
        avl.init(values);

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
                if (!values.empty()) avl.init(values);

                Visualizer::Instance().SetStep(0);
                Visualizer::Instance().SetPlaying(true);
            }
        }
    }
}

void AVLState::DrawAddMenu(float x, float y) {
    DrawRectangle(x, y, 200, 120, RAYWHITE);
    DrawRectangleLines(x, y, 200, 120, GRAY);

    DrawText("Value:", x + 10, y + 25, 20, BLACK);
    if (GuiTextBox((Rectangle){x + 80, y + 20, 80, 30}, inputBuffer, 64, isInputActive)) isInputActive = !isInputActive;

    if (GuiButton((Rectangle){x + 10, y + 70, 150, 30}, "Insert Value") || IsKeyPressed(KEY_ENTER)) {
        if (inputBuffer[0] != '\0') {
            Visualizer::Instance().ClearHistory();
            Visualizer::Instance().RecordState("Initial State", 0, avl.captureState(), {});
            avl.insert(atoi(inputBuffer));
            Visualizer::Instance().SetStep(0);
            Visualizer::Instance().SetPlaying(true);

            // Clean up UI
            inputBuffer[0] = '\0';
            isInputActive = false;
        }
    }
}

void AVLState::DrawDeleteMenu(float x, float y) {
    DrawRectangle(x, y, 200, 120, RAYWHITE);
    DrawRectangleLines(x, y, 200, 120, GRAY);

    DrawText("Value:", x + 10, y + 25, 20, BLACK);
    if (GuiTextBox((Rectangle){x + 80, y + 20, 80, 30}, inputBuffer, 64, isInputActive)) isInputActive = !isInputActive;

    if (GuiButton((Rectangle){x + 10, y + 70, 150, 30}, "Delete Value") || IsKeyPressed(KEY_ENTER)) {
        if (inputBuffer[0] != '\0') {
            Visualizer::Instance().ClearHistory();
            Visualizer::Instance().RecordState("Initial State", 0, avl.captureState(), {});
            avl.deleteNode(atoi(inputBuffer));
            Visualizer::Instance().SetStep(0);
            Visualizer::Instance().SetPlaying(true);

            // Clean up UI
            inputBuffer[0] = '\0';
            isInputActive = false;
        }
    }
}

void AVLState::DrawUpdateMenu(float x, float y) {
    DrawRectangle(x, y, 350, 120, RAYWHITE);
    DrawRectangleLines(x, y, 350, 120, GRAY);

    DrawText("Old Val:", x + 10, y + 25, 20, BLACK);
    if (GuiTextBox((Rectangle){x + 90, y + 20, 60, 30}, oldBuffer, 64, isOldActive)) isOldActive = !isOldActive;

    DrawText("New Val:", x + 160, y + 25, 20, BLACK);
    if (GuiTextBox((Rectangle){x + 250, y + 20, 80, 30}, inputBuffer, 64, isInputActive)) isInputActive = !isInputActive;

    if (GuiButton((Rectangle){x + 10, y + 70, 320, 30}, "Update Value") || IsKeyPressed(KEY_ENTER)) {
        if (oldBuffer[0] != '\0' && inputBuffer[0] != '\0') {
            Visualizer::Instance().ClearHistory();
            Visualizer::Instance().RecordState("Initial State", 0, avl.captureState(), {});
            avl.updateNode(atoi(oldBuffer), atoi(inputBuffer));
            Visualizer::Instance().SetStep(0);
            Visualizer::Instance().SetPlaying(true);

            // Clean up UI
            oldBuffer[0] = '\0';
            inputBuffer[0] = '\0';
            isOldActive = false;
            isInputActive = false;
        }
    }
}

void AVLState::DrawSearchMenu(float x, float y) {
    DrawRectangle(x, y, 200, 120, RAYWHITE);
    DrawRectangleLines(x, y, 200, 120, GRAY);

    DrawText("Value:", x + 10, y + 25, 20, BLACK);
    if (GuiTextBox((Rectangle){x + 80, y + 20, 80, 30}, inputBuffer, 64, isInputActive)) isInputActive = !isInputActive;

    if (GuiButton((Rectangle){x + 10, y + 70, 150, 30}, "Search Value") || IsKeyPressed(KEY_ENTER)) {
        if (inputBuffer[0] != '\0') {
            Visualizer::Instance().ClearHistory();
            Visualizer::Instance().RecordState("Initial State", 0, avl.captureState(), {});
            avl.searchNode(atoi(inputBuffer));
            Visualizer::Instance().SetStep(0);
            Visualizer::Instance().SetPlaying(true);

            // Clean up UI
            inputBuffer[0] = '\0';
            isInputActive = false;
        }
    }
}
void AVLState::DrawPlayback() {
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

void AVLState::DrawPseudocode() {
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