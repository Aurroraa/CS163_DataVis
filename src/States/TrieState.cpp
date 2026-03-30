#include "../../include/States/TrieState.h"
#include "../../include/States/SelectState.h"
#include "../../include/App.h"
#include "../../include/raygui.h"
#include "../../include/Visualizer.h"
#include "../../include/tinyfiledialogs.h"
#include "../../include/Renderers/TrieRenderer.h"
#include <cstdlib>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

TrieState::TrieState() {
    Visualizer::Instance().ClearHistory();
    trie.init({}); // Example words!
    playbackSpeed = 0.5f;
}

void TrieState::Init() {
    showAddMenu = showDeleteMenu = showInitMenu = showSearchMenu = showUpdateMenu = false;
    inputBuffer[0] = '\0';
    sizeBuffer[0] = '\0';
    Visualizer::Instance().SetSpeed(playbackSpeed);
}

void TrieState::Update() {Visualizer::Instance().Update();}

void TrieState::Draw() {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight() - 200, RAYWHITE);
    Visualizer::Instance().DrawCanvas();

    if (GuiButton((Rectangle){(float)GetScreenWidth() - 100, 10, 80, 30}, "Home")) {
        g_App->ChangeState(new SelectState());
    }

    TrieRenderer::Draw(Visualizer::Instance().GetRenderState());
    DrawToolbar(); DrawPlayback(); DrawPseudocode();
}

void TrieState::DrawToolbar() {
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

void TrieState::DrawInitMenu(float x, float y) {
    DrawRectangle(x, y, 620, 120, RAYWHITE);
    DrawRectangleLines(x, y, 620, 120, GRAY);

    // --- ROW 1: Clear & Manual Input ---
    if (GuiButton((Rectangle){x + 10, y + 10, 120, 30}, "Empty (Clear)")) {
        Visualizer::Instance().ClearHistory();
        trie.init({});
        Visualizer::Instance().SetPlaying(false);
    }

    DrawText("Words (csv):", x + 150, y + 15, 20, BLACK);
    if (GuiTextBox((Rectangle){x + 280, y + 10, 200, 30}, inputBuffer, 64, isInputActive)) isInputActive = !isInputActive;

    if (GuiButton((Rectangle){x + 490, y + 10, 40, 30}, "Go") || (IsKeyPressed(KEY_ENTER))) {
        if (inputBuffer[0] != '\0') {
            Visualizer::Instance().ClearHistory();
            std::vector<std::string> words;
            std::stringstream ss(inputBuffer);
            std::string seg;
            while(std::getline(ss, seg, ',')) words.push_back(seg);
            if (!words.empty()) trie.init(words);

            Visualizer::Instance().SetStep(0);
            Visualizer::Instance().SetPlaying(true);

            inputBuffer[0] = '\0';
            isInputActive = false;
        }
    }

    // --- ROW 2: Random Generation & File Browser ---
    DrawText("Random N =", x + 10, y + 65, 20, BLACK);
    if (GuiTextBox((Rectangle){x + 130, y + 60, 50, 30}, nBuffer, 16, nInputActive)) nInputActive = !nInputActive;

    // 🌟 THE NEW RANDOM WORD GENERATOR
    if (GuiButton((Rectangle){x + 190, y + 60, 90, 30}, "Generate") || (IsKeyPressed(KEY_ENTER))) {
        if (nBuffer[0] != '\0') {
            Visualizer::Instance().ClearHistory();

            int n = atoi(nBuffer);
            if (n < 1) n = 1;
            if (n > 10) n = 10;

            std::vector<std::string> words;
            for(int i = 0; i < n; i++) {
                int wordLength = GetRandomValue(3, 6);
                std::string randomWord = "";
                for(int j = 0; j < wordLength; j++) {
                    randomWord += (char)GetRandomValue('a', 'z');
                }
                words.push_back(randomWord);
            }

            trie.init(words);

            Visualizer::Instance().SetStep(0);
            Visualizer::Instance().SetPlaying(true);

            nBuffer[0] = '\0';
            nInputActive = false;
        }
    }

    if (GuiButton((Rectangle){x + 300, y + 60, 120, 30}, "Browse File...")) {
        const char *filterPatterns[1] = { "*.txt" };
        const char *filePath = tinyfd_openFileDialog("Select Input File", "", 1, filterPatterns, "Text Files", 0);
        if (filePath != NULL) {
            std::ifstream file(filePath);
            if (file.is_open()) {
                std::vector<std::string> words;
                std::string word;
                while (file >> word) {
                    // Remove commas if they read a CSV formatted text file
                    word.erase(std::remove(word.begin(), word.end(), ','), word.end());
                    if (!word.empty()) words.push_back(word);
                }
                file.close();
                if (!words.empty()) trie.init(words);

                Visualizer::Instance().SetStep(0);
                Visualizer::Instance().SetPlaying(true);
            }
        }
    }
}

void TrieState::DrawAddMenu(float x, float y) {
    DrawRectangle(x, y, 200, 120, RAYWHITE);
    DrawRectangleLines(x, y, 200, 120, GRAY);

    DrawText("Word:", x + 10, y + 25, 20, BLACK);
    if (GuiTextBox((Rectangle){x + 80, y + 20, 100, 30}, inputBuffer, 64, isInputActive)) isInputActive = !isInputActive;

    if (GuiButton((Rectangle){x + 10, y + 70, 170, 30}, "Insert Word") || (IsKeyPressed(KEY_ENTER))) {
        if (inputBuffer[0] != '\0') {
            Visualizer::Instance().ClearHistory();
            Visualizer::Instance().RecordState("Initial State", 0, trie.captureState(), {});
            trie.insert(inputBuffer);
            Visualizer::Instance().SetStep(0); Visualizer::Instance().SetPlaying(true);
            inputBuffer[0] = '\0'; isInputActive = false;
        }
    }
}

void TrieState::DrawDeleteMenu(float x, float y) {
    DrawRectangle(x, y, 200, 120, RAYWHITE);
    DrawRectangleLines(x, y, 200, 120, GRAY);

    DrawText("Word:", x + 10, y + 25, 20, BLACK);
    if (GuiTextBox((Rectangle){x + 80, y + 20, 100, 30}, inputBuffer, 64, isInputActive)) isInputActive = !isInputActive;

    if (GuiButton((Rectangle){x + 10, y + 70, 170, 30}, "Delete Word") || (IsKeyPressed(KEY_ENTER))) {
        if (inputBuffer[0] != '\0') {
            Visualizer::Instance().ClearHistory();
            Visualizer::Instance().RecordState("Initial State", 0, trie.captureState(), {});
            trie.deleteWord(inputBuffer);
            Visualizer::Instance().SetStep(0); Visualizer::Instance().SetPlaying(true);
            inputBuffer[0] = '\0'; isInputActive = false;
        }
    }
}

void TrieState::DrawSearchMenu(float x, float y) {
    DrawRectangle(x, y, 200, 120, RAYWHITE);
    DrawRectangleLines(x, y, 200, 120, GRAY);

    DrawText("Word:", x + 10, y + 25, 20, BLACK);
    if (GuiTextBox((Rectangle){x + 80, y + 20, 100, 30}, inputBuffer, 64, isInputActive)) isInputActive = !isInputActive;

    if (GuiButton((Rectangle){x + 10, y + 70, 170, 30}, "Search Word") || (IsKeyPressed(KEY_ENTER))) {
        if (inputBuffer[0] != '\0') {
            Visualizer::Instance().ClearHistory();
            Visualizer::Instance().RecordState("Initial State", 0, trie.captureState(), {});
            trie.searchWord(inputBuffer);
            Visualizer::Instance().SetStep(0); Visualizer::Instance().SetPlaying(true);
            inputBuffer[0] = '\0'; isInputActive = false;
        }
    }
}

void TrieState::DrawUpdateMenu(float x, float y) {
    DrawRectangle(x, y, 350, 120, RAYWHITE);
    DrawRectangleLines(x, y, 350, 120, GRAY);

    DrawText("Word:", x + 10, y + 25, 20, BLACK);
    if (GuiTextBox((Rectangle){x + 70, y + 20, 80, 30}, inputBuffer, 64, isInputActive)) isInputActive = !isInputActive;

    DrawText("Size:", x + 170, y + 25, 20, BLACK);
    if (GuiTextBox((Rectangle){x + 230, y + 20, 60, 30}, sizeBuffer, 64, isSizeActive)) isSizeActive = !isSizeActive;

    if (GuiButton((Rectangle){x + 10, y + 70, 320, 30}, "Update Word Size") || (IsKeyPressed(KEY_ENTER))) {
        if (inputBuffer[0] != '\0' && sizeBuffer[0] != '\0') {
            Visualizer::Instance().ClearHistory();
            Visualizer::Instance().RecordState("Initial State", 0, trie.captureState(), {});

            trie.updateWord(inputBuffer, atoi(sizeBuffer));

            Visualizer::Instance().SetStep(0);
            Visualizer::Instance().SetPlaying(true);

            // Clean up UI
            inputBuffer[0] = '\0';
            sizeBuffer[0] = '\0';
            isInputActive = false;
            isSizeActive = false;
        }
    }
}

void TrieState::DrawPlayback() {
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

void TrieState::DrawPseudocode() {
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


