#include "../../include/States/DLLState.h"
#include "../../include/States/SelectState.h"
#include "../../include/App.h"
#include "../../include/raygui.h"
#include "../../include/Visualizer.h"
#include "../../include/tinyfiledialogs.h"
#include "../../include/Renderers/DLLRenderer.h"
#include <cstdlib>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>

DLLState::DLLState() {
    dll.init({10, 20, 30});
    playbackSpeed = 0.5f;
}

void DLLState::Init() {
    showInitMenu = false;
    showAddMenu = false;
    showDeleteMenu = false;
    showUpdateMenu = false;
    showSearchMenu = false;
    inputBuffer[0] = '\0';
    locBuffer[0] = '\0';
    Visualizer::Instance().SetSpeed(playbackSpeed);
}

void DLLState::Update() {
    Visualizer::Instance().Update();
}

void DLLState::Draw() {
    // 1. Canvas restored to float above the toolbar
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight() - 200, RAYWHITE);
    Visualizer::Instance().DrawCanvas();

    // 2. Navigation
    if (GuiButton((Rectangle){(float)GetScreenWidth() - 100, 10, 80, 30}, "Home")) {
        g_App->ChangeState(new SelectState());
    }

    // 3. Draw Data Structure
    DLLRenderer::Draw(Visualizer::Instance().GetRenderState());

    // 4. UI Components
    DrawToolbar();
    DrawPlayback();
    DrawPseudocode();
}

void DLLState::DrawToolbar() {
    float startY = GetScreenHeight() - 200;

    // Draw Toolbar Background
    DrawRectangle(0, startY, GetScreenWidth(), 200, LIGHTGRAY);
    DrawLine(0, startY, GetScreenWidth(), startY, DARKGRAY);

    int btnWidth = 100;
    int gap = 10;
    float x = 20;

    // Macro to toggle menus nicely
    #define CLOSE_ALL_MENUS() showInitMenu = showAddMenu = showDeleteMenu = showUpdateMenu = showSearchMenu = false;

    // --- ROW 1: THE MAIN BUTTONS ---
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

    // --- ROW 2: THE POPUPS ---
    // Drawn neatly inside the toolbar below the buttons!
    if (showInitMenu)   DrawInitMenu(20, startY + 60);
    if (showAddMenu)    DrawAddMenu(20, startY + 60);
    if (showDeleteMenu) DrawDeleteMenu(20, startY + 60);
    if (showUpdateMenu) DrawUpdateMenu(20, startY + 60);
    if (showSearchMenu) DrawSearchMenu(20, startY + 60);
}

// ---------------------------------------------------------
// FLATTENED POPUP SCREENS (To fit inside the 200px Toolbar)
// ---------------------------------------------------------
void DLLState::DrawInitMenu(float x, float y) {
    DrawRectangle(x, y, 620, 120, RAYWHITE);
    DrawRectangleLines(x, y, 620, 120, GRAY);

    // Row 1: Clear & User Input
    if (GuiButton((Rectangle){x + 10, y + 10, 120, 30}, "Empty (Clear)")) {
        Visualizer::Instance().ClearHistory();
        dll.init({});
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
        if (!values.empty()) dll.init(values);

        Visualizer::Instance().SetStep(0);
        Visualizer::Instance().SetPlaying(true);

        inputBuffer[0] = '\0';
        isInputActive = false;
    }

    // Row 2: Random & Browse File
    DrawText("Random N =", x + 10, y + 65, 20, BLACK);
    if (GuiTextBox((Rectangle){x + 130, y + 60, 50, 30}, nBuffer, 16, nInputActive)) nInputActive = !nInputActive;
    if (GuiButton((Rectangle){x + 190, y + 60, 90, 30}, "Generate") || (IsKeyPressed(KEY_ENTER))) {
        Visualizer::Instance().ClearHistory();
        int n = atoi(nBuffer);
        if (n < 1) n = 1; if (n > 15) n = 15;
        std::vector<int> values;
        for(int i = 0; i < n; i++) values.push_back(GetRandomValue(1, 99));
        dll.init(values);

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
                if (!values.empty()) dll.init(values);

                Visualizer::Instance().SetStep(0);
                Visualizer::Instance().SetPlaying(true);

            }
        }
    }
}

void DLLState::DrawAddMenu(float x, float y) {
    DrawRectangle(x, y, 400, 120, RAYWHITE);
    DrawRectangleLines(x, y, 400, 120, GRAY);

    DrawText("Value:", x + 10, y + 25, 20, BLACK);
    if (GuiTextBox((Rectangle){x + 80, y + 20, 80, 30}, inputBuffer, 64, isInputActive)) isInputActive = !isInputActive;

    DrawText("Loc:", x + 180, y + 25, 20, BLACK);
    if (GuiTextBox((Rectangle){x + 230, y + 20, 80, 30}, locBuffer, 64, isLocActive)) isLocActive = !isLocActive;

    if (GuiButton((Rectangle){x + 10, y + 70, 60, 30}, "Head")) {
        int start = Visualizer::Instance().GetTotalSteps();
        dll.addHead(atoi(inputBuffer));
        Visualizer::Instance().SetStep(start); Visualizer::Instance().SetPlaying(true);
        inputBuffer[0] = '\0';
        isInputActive = false;

        locBuffer[0] = '\0';
        isLocActive = false;
    }
    if (GuiButton((Rectangle){x + 80, y + 70, 60, 30}, "Tail")) {
        int start = Visualizer::Instance().GetTotalSteps();
        dll.addTail(atoi(inputBuffer));
        Visualizer::Instance().SetStep(start); Visualizer::Instance().SetPlaying(true);

        inputBuffer[0] = '\0';
        isInputActive = false;

        locBuffer[0] = '\0';
        isLocActive = false;
    }
    if (GuiButton((Rectangle){x + 150, y + 70, 90, 30}, "Location")) {
        int start = Visualizer::Instance().GetTotalSteps();
        dll.addAtIndex(atoi(locBuffer), atoi(inputBuffer));
        Visualizer::Instance().SetStep(start); Visualizer::Instance().SetPlaying(true);

        inputBuffer[0] = '\0';
        isInputActive = false;

        locBuffer[0] = '\0';
        isLocActive = false;
    }
}

void DLLState::DrawDeleteMenu(float x, float y) {
    DrawRectangle(x, y, 300, 120, RAYWHITE);
    DrawRectangleLines(x, y, 300, 120, GRAY);

    DrawText("Loc:", x + 10, y + 25, 20, BLACK);
    if (GuiTextBox((Rectangle){x + 60, y + 20, 80, 30}, locBuffer, 64, isLocActive)) isLocActive = !isLocActive;

    if (GuiButton((Rectangle){x + 10, y + 70, 60, 30}, "Head")) {
        int start = Visualizer::Instance().GetTotalSteps();
        dll.deleteHead();
        Visualizer::Instance().SetStep(start); Visualizer::Instance().SetPlaying(true);

        locBuffer[0] = '\0';
        isLocActive = false;
    }
    if (GuiButton((Rectangle){x + 80, y + 70, 60, 30}, "Tail")) {
        int start = Visualizer::Instance().GetTotalSteps();
        dll.deleteTail();
        Visualizer::Instance().SetStep(start); Visualizer::Instance().SetPlaying(true);

        locBuffer[0] = '\0';
        isLocActive = false;
    }
    if (GuiButton((Rectangle){x + 150, y + 70, 90, 30}, "Location")|| (IsKeyPressed(KEY_ENTER))) {
        int start = Visualizer::Instance().GetTotalSteps();
        dll.deleteAtIndex(atoi(locBuffer));
        Visualizer::Instance().SetStep(start); Visualizer::Instance().SetPlaying(true);

        locBuffer[0] = '\0';
        isLocActive = false;
    }
}

void DLLState::DrawUpdateMenu(float x, float y) {
    DrawRectangle(x, y, 350, 120, RAYWHITE);
    DrawRectangleLines(x, y, 350, 120, GRAY);

    DrawText("Loc:", x + 10, y + 25, 20, BLACK);
    if (GuiTextBox((Rectangle){x + 60, y + 20, 60, 30}, locBuffer, 64, isLocActive)) isLocActive = !isLocActive;

    DrawText("New Val:", x + 140, y + 25, 20, BLACK);
    if (GuiTextBox((Rectangle){x + 230, y + 20, 80, 30}, inputBuffer, 64, isInputActive)) isInputActive = !isInputActive;

    if (GuiButton((Rectangle){x + 10, y + 70, 200, 30}, "Update Location") || (IsKeyPressed(KEY_ENTER))) {
        int start = Visualizer::Instance().GetTotalSteps();
        dll.updateAtIndex(atoi(locBuffer), atoi(inputBuffer));
        Visualizer::Instance().SetStep(start); Visualizer::Instance().SetPlaying(true);
        inputBuffer[0] = '\0';
        isInputActive = false;
        locBuffer[0] = '\0';
        isLocActive = false;
    }
}

void DLLState::DrawSearchMenu(float x, float y) {
    DrawRectangle(x, y, 250, 120, RAYWHITE);
    DrawRectangleLines(x, y, 250, 120, GRAY);

    DrawText("Value:", x + 10, y + 25, 20, BLACK);
    if (GuiTextBox((Rectangle){x + 80, y + 20, 80, 30}, inputBuffer, 64, isInputActive)) isInputActive = !isInputActive;

    if (GuiButton((Rectangle){x + 10, y + 70, 180, 30}, "Search Value") || (IsKeyPressed(KEY_ENTER))) {
        int start = Visualizer::Instance().GetTotalSteps();
        dll.searchNode(atoi(inputBuffer));
        Visualizer::Instance().SetStep(start); Visualizer::Instance().SetPlaying(true);

        inputBuffer[0] = '\0';
        isInputActive = false;
    }
}

void DLLState::DrawPlayback() {
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

void DLLState::DrawPseudocode() {
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