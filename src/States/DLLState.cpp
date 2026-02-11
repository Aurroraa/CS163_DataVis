#include "../../include/States/DLLState.h"
#include "../../include/States/SelectState.h"
#include "../../include/App.h"
#include "../../include/raygui.h"
#include "../../include/Visualizer.h"
#include <cstdlib>
#include <sstream>
#include "../../include/tinyfiledialogs.h"
#include <fstream>
#include <vector>
#include <string>

DLLState::DLLState() {
    dll.init({10, 20, 30});
    playbackSpeed = 0.5f; // Initialize default speed
}

void DLLState::Init() {
    showCreateMenu = false;
    showInsertMenu = false;
    showDeleteMenu = false;
    inputBuffer[0] = '\0';
    // Ensure Visualizer is synced with default speed
    Visualizer::Instance().SetSpeed(playbackSpeed);
}

void DLLState::Update() {
    Visualizer::Instance().Update();
}

void DLLState::Draw() {
    // 1. Canvas
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight() - 200, RAYWHITE);
    Visualizer::Instance().DrawCanvas();

    // 2. Navigation
    if (GuiButton((Rectangle){(float)GetScreenWidth() - 100, 10, 80, 30}, "Home")) {
        g_App->ChangeState(new SelectState());
    }

    // 3. UI Components
    DrawToolbar();
    DrawPlayback();
    DrawPseudocode();
}

void DLLState::DrawToolbar() {
    float startY = GetScreenHeight() - 200;
    DrawRectangle(0, startY, GetScreenWidth(), 200, LIGHTGRAY);
    DrawLine(0, startY, GetScreenWidth(), startY, DARKGRAY);

    int btnWidth = 100; int gap = 10; int x = 20;

    // 1. Button: CREATE
    if (GuiButton((Rectangle){(float)x, startY + 10, (float)btnWidth, 40}, "Create")) {
        showCreateMenu = !showCreateMenu;
        showInsertMenu = false; showDeleteMenu = false;
    }
    x += btnWidth + gap;

    // 2. Button: INSERT
    if (GuiButton((Rectangle){(float)x, startY + 10, (float)btnWidth, 40}, "Insert")) {
        showInsertMenu = !showInsertMenu;
        showCreateMenu = false; showDeleteMenu = false;
    }
    x += btnWidth + gap;

    // 3. Button: DELETE
    if (GuiButton((Rectangle){(float)x, startY + 10, (float)btnWidth, 40}, "Delete")) {
        showDeleteMenu = !showDeleteMenu;
        showCreateMenu = false; showInsertMenu = false;
    }
    x += btnWidth + gap;

    // --- NEW: Button: CLEAR ---
    if (GuiButton((Rectangle){(float)x, startY + 10, (float)btnWidth, 40}, "Clear")) {
        // 1. Wipe the history (removes all steps/notes)
        Visualizer::Instance().ClearHistory();

        // 2. Reset the Data Structure to empty
        // Passing an empty vector {} creates a blank list
        dll.init({});

        // 3. Reset UI state
        showCreateMenu = false;
        showInsertMenu = false;
        showDeleteMenu = false;
        Visualizer::Instance().SetPlaying(false);
    }

    // --- Sub Menus (Logic remains the same) ---
    if (showCreateMenu) DrawCreateMenu(20, startY + 60);
    else if (showInsertMenu) DrawInsertMenu(130, startY + 60);
    else if (showDeleteMenu) DrawDeleteMenu(240, startY + 60);
}

void DLLState::DrawCreateMenu(float x, float y) {
    // ---------------------------------------------------------
    // ROW 1: Manual Input (e.g. "10,20,30")
    // ---------------------------------------------------------
    DrawText("Values (e.g. 5,10):", x, y + 5, 20, BLACK);

    // Text Box for CSV
    if (GuiTextBox((Rectangle){x + 200, y, 100, 30}, inputBuffer, 64, isInputActive)) {
        isInputActive = !isInputActive;
    }

    // Button: Go (Manual)
    if (GuiButton((Rectangle){x + 310, y, 40, 30}, "Go")) {
        Visualizer::Instance().ClearHistory(); // Clear old data

        std::vector<int> values;
        std::stringstream ss(inputBuffer);
        std::string segment;
        while(std::getline(ss, segment, ',')) {
            try { values.push_back(std::stoi(segment)); } catch(...) {}
        }

        if (!values.empty()) dll.init(values);

        Visualizer::Instance().SetStep(0);
        Visualizer::Instance().SetPlaying(false);
    }

    // ---------------------------------------------------------
    // ROW 2: Generate N Random Nodes
    // ---------------------------------------------------------
    y += 40; // Move down one row

    DrawText("Random Size (N):", x, y + 5, 20, BLACK);

    // New Input for N (using a static buffer so we don't need to change .h file)
    static char nBuffer[16] = "5"; // Default to 5
    static bool nInputActive = false;

    if (GuiTextBox((Rectangle){x + 200, y, 50, 30}, nBuffer, 16, nInputActive)) {
        nInputActive = !nInputActive;
    }

    // Button: Generate Random
    if (GuiButton((Rectangle){x + 260, y, 90, 30}, "Generate")) {
        Visualizer::Instance().ClearHistory(); // Clear old data

        int n = atoi(nBuffer);

        // Safety Limit: Don't crash with 1000 nodes
        if (n < 1) n = 1;
        if (n > 15) n = 15; // Limit to 20 for screen space

        std::vector<int> values;
        for(int i = 0; i < n; i++) {
            values.push_back(GetRandomValue(1, 99));
        }

        dll.init(values);

        Visualizer::Instance().SetStep(0);
        Visualizer::Instance().SetPlaying(false);
    }

    // ---------------------------------------------------------
    // ROW 3: File Loading
    // ---------------------------------------------------------
    y += 40; // Move down again
    // C. Load File
    if (GuiButton((Rectangle){x, y, 120, 30}, "Browse...")) {

        // A. Define file filters (e.g., only .txt files)
        const char *filterPatterns[1] = { "*.txt" };

        // B. Open the Dialog
        // Args: Title, Default Path, Num Filters, Filter Patterns, Description, Allow Multi-select
        const char *filePath = tinyfd_openFileDialog(
            "Select Input File",
            "",
            1,
            filterPatterns,
            "Text Files",
            0
        );

        // C. If user selected a file (filePath is not NULL)
        if (filePath != NULL) {
            std::ifstream file(filePath);
            if (file.is_open()) {
                std::vector<int> values;
                int val;
                std::string temp;

                // Read integers from file (ignores commas/spaces)
                while (file >> val) {
                    values.push_back(val);
                    // Optional: handle commas if your file format is "1,2,3"
                    if (file.peek() == ',') file.ignore();
                }

                file.close();

                // Initialize the list if we found data
                if (!values.empty()) {
                    dll.init(values);
                }
            }
        }
    }
}

void DLLState::DrawInsertMenu(float x, float y) {
    DrawText("Value:", x, y + 5, 20, BLACK);
    if (GuiTextBox((Rectangle){x + 60, y, 80, 30}, inputBuffer, 64, isInputActive)) {
        isInputActive = !isInputActive;
    }

    // BUTTON: Insert Head
    if (GuiButton((Rectangle){x, y + 40, 60, 30}, "Head")) {
        int startStep = Visualizer::Instance().GetTotalSteps(); // 1. Save Start

        dll.addHead(atoi(inputBuffer)); // 2. Run Logic (fills history)

        Visualizer::Instance().SetStep(startStep); // 3. Jump to Start of Animation
        Visualizer::Instance().SetPlaying(true);   // 4. Auto-Play
    }

    // BUTTON: Insert Tail
    if (GuiButton((Rectangle){x + 70, y + 40, 60, 30}, "Tail")) {
        int startStep = Visualizer::Instance().GetTotalSteps();
        dll.addTail(atoi(inputBuffer));
        Visualizer::Instance().SetStep(startStep);
        Visualizer::Instance().SetPlaying(true);
    }
}

void DLLState::DrawDeleteMenu(float x, float y) {
    DrawText("Value:", x, y + 5, 20, BLACK);
    if (GuiTextBox((Rectangle){x + 60, y, 80, 30}, inputBuffer, 64, isInputActive)) {
        isInputActive = !isInputActive;
    }

    // BUTTON: Delete
    if (GuiButton((Rectangle){x, y + 40, 100, 30}, "Delete Node")) {
        int startStep = Visualizer::Instance().GetTotalSteps();
        dll.deleteNode(atoi(inputBuffer));
        Visualizer::Instance().SetStep(startStep);
        Visualizer::Instance().SetPlaying(true);
    }
}

void DLLState::DrawPlayback() {
    int centerX = GetScreenWidth() / 2;
    int y = GetScreenHeight() - 240;

    // 1. Navigation Buttons
    if (GuiButton((Rectangle){(float)centerX - 100, (float)y, 40, 30}, "<<")) Visualizer::Instance().GoToStart();
    if (GuiButton((Rectangle){(float)centerX - 50, (float)y, 40, 30}, "<")) Visualizer::Instance().PrevStep();

    // 2. Play / Pause Button
    const char* label = Visualizer::Instance().IsPlaying() ? "||" : ">";
    if (GuiButton((Rectangle){(float)centerX, (float)y, 40, 30}, label)) {
        Visualizer::Instance().TogglePlay();
    }

    if (GuiButton((Rectangle){(float)centerX + 50, (float)y, 40, 30}, ">")) Visualizer::Instance().NextStep();
    if (GuiButton((Rectangle){(float)centerX + 100, (float)y, 40, 30}, ">>")) Visualizer::Instance().GoToEnd();

    // 3. Speed Slider (Fixed)
    DrawText("Speed:", centerX + 160, y + 5, 20, BLACK);

    // FIX: Using member variable 'playbackSpeed'
    // FIX: Range is 0.1 (Left/Fast) to 2.0 (Right/Slow)
    // We update the Visualizer every frame to ensure it stays in sync
    GuiSlider((Rectangle){(float)centerX + 270, (float)y, 100, 30}, "Fast", "Slow", &playbackSpeed, 0.1f, 2.0f);

    // Always apply the speed
    Visualizer::Instance().SetSpeed(playbackSpeed);
}

void DLLState::DrawPseudocode() {
    int panelW = 400;
    int panelH = 200;
    int startX = GetScreenWidth() - panelW;
    int startY = GetScreenHeight() - panelH;

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