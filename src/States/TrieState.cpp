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
    // 1. Themed Canvas
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight() - 200, UIHelper::GetCanvasBg(config));
    Visualizer::Instance().DrawCanvas();

    Color textCol = UIHelper::GetTextCol(config);
    float btnX = GetScreenWidth() - 150;

    // Home Button
    float homeBtnX = btnX - 110;
    if (UIHelper::DrawModernBtn({homeBtnX, 10, 90, 36}, "Home", false, config)) {
        g_App->ChangeState(new SelectState());
    }

    // Hamburger Menu
    if (UIHelper::DrawHamburgerBtn({btnX, 10, 40, 36}, config)) isSettingsOpen = !isSettingsOpen;

    // 3. Draw Data Structure
    TrieRenderer::Draw(Visualizer::Instance().GetRenderState(), config);

    DrawToolbar();
    DrawPlayback();
    DrawPseudocode();

    if (isSettingsOpen) DrawSettingsModal();
}

void TrieState::DrawToolbar() {
    float startY = GetScreenHeight() - 200;

    DrawRectangle(0, startY, GetScreenWidth(), 200, UIHelper::GetPanelBg(config));
    DrawLine(0, startY, GetScreenWidth(), startY, UIHelper::GetOutlineCol(config));

    int btnWidth = 100; int gap = 10; float x = 20;
    #define CLOSE_ALL_MENUS() showInitMenu = showAddMenu = showDeleteMenu = showUpdateMenu = showSearchMenu = false;

    // --- ROW 1: THE MAIN BUTTONS ---
    if (UIHelper::DrawModernBtn({x, startY + 10, (float)btnWidth, 40}, "Init", showInitMenu, config)) {
        bool wasOpen = showInitMenu; CLOSE_ALL_MENUS(); showInitMenu = !wasOpen;
    } x += btnWidth + gap;

    if (UIHelper::DrawModernBtn({x, startY + 10, (float)btnWidth, 40}, "Add", showAddMenu, config)) {
        bool wasOpen = showAddMenu; CLOSE_ALL_MENUS(); showAddMenu = !wasOpen;
    } x += btnWidth + gap;

    if (UIHelper::DrawModernBtn({x, startY + 10, (float)btnWidth, 40}, "Delete", showDeleteMenu, config)) {
        bool wasOpen = showDeleteMenu; CLOSE_ALL_MENUS(); showDeleteMenu = !wasOpen;
    } x += btnWidth + gap;

    if (UIHelper::DrawModernBtn({x, startY + 10, (float)btnWidth, 40}, "Update", showUpdateMenu, config)) {
        bool wasOpen = showUpdateMenu; CLOSE_ALL_MENUS(); showUpdateMenu = !wasOpen;
    } x += btnWidth + gap;

    if (UIHelper::DrawModernBtn({x, startY + 10, (float)btnWidth, 40}, "Search", showSearchMenu, config)) {
        bool wasOpen = showSearchMenu; CLOSE_ALL_MENUS(); showSearchMenu = !wasOpen;
    }

    // --- ROW 2: THE POPUPS ---
    if (showInitMenu)   DrawInitMenu(20, startY + 60);
    if (showAddMenu)    DrawAddMenu(20, startY + 60);
    if (showDeleteMenu) DrawDeleteMenu(20, startY + 60);
    if (showUpdateMenu) DrawUpdateMenu(20, startY + 60);
    if (showSearchMenu) DrawSearchMenu(20, startY + 60);
}

void TrieState::DrawInitMenu(float x, float y) {
    Color panelBg = UIHelper::GetCanvasBg(config);
    Color outlineCol = UIHelper::GetOutlineCol(config);
    Color textCol = UIHelper::GetTextCol(config);

    DrawRectangleRounded({x, y, 620, 120}, 0.1f, 8, panelBg);
    DrawRectangleRoundedLines({x, y, 620, 120}, 0.1f, 8, outlineCol);

    // --- ROW 1: Clear & Manual Input ---
    if (UIHelper::DrawModernBtn({x + 10, y + 10, 120, 30}, "Empty (Clear)", true, config)) {
        Visualizer::Instance().ClearHistory();
        trie.init({});
        Visualizer::Instance().SetPlaying(false);
    }

    DrawText("Words (csv):", x + 150, y + 15, 20, textCol);
    if (GuiTextBox((Rectangle){x + 280, y + 10, 200, 30}, inputBuffer, 64, isInputActive)) isInputActive = !isInputActive;

    if (UIHelper::DrawModernBtn({x + 490, y + 10, 40, 30}, "Go", true, config) || (IsKeyPressed(KEY_ENTER))) {
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
    DrawText("Random N =", x + 10, y + 65, 20, textCol);
    if (GuiTextBox((Rectangle){x + 130, y + 60, 50, 30}, nBuffer, 16, nInputActive)) nInputActive = !nInputActive;

    // 🌟 THE NEW RANDOM WORD GENERATOR
    if (UIHelper::DrawModernBtn({x + 190, y + 60, 90, 30}, "Generate", true, config) || (IsKeyPressed(KEY_ENTER))) {
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

    if (UIHelper::DrawModernBtn({x + 300, y + 60, 120, 30}, "Browse File...", true, config)) {
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
    Color panelBg = UIHelper::GetCanvasBg(config);
    Color outlineCol = UIHelper::GetOutlineCol(config);
    Color textCol = UIHelper::GetTextCol(config);

    DrawRectangleRounded({x, y, 280, 120}, 0.1f, 8, panelBg);
    DrawRectangleRoundedLines({x, y, 280, 120}, 0.1f, 8, outlineCol);

    DrawText("Word:", x + 10, y + 25, 20, textCol);
    if (GuiTextBox((Rectangle){x + 80, y + 20, 100, 30}, inputBuffer, 64, isInputActive)) isInputActive = !isInputActive;

    if (UIHelper::DrawModernBtn({x + 10, y + 70, 260, 35}, "Insert Word", true, config) || IsKeyPressed(KEY_ENTER)) {
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
    Color panelBg = UIHelper::GetCanvasBg(config);
    Color outlineCol = UIHelper::GetOutlineCol(config);
    Color textCol = UIHelper::GetTextCol(config);

    DrawRectangleRounded({x, y, 200, 120}, 0.1f, 8, panelBg);
    DrawRectangleRoundedLines({x, y, 200, 120}, 0.1f, 8, outlineCol);

    DrawText("Word:", x + 10, y + 25, 20, textCol);
    if (GuiTextBox((Rectangle){x + 80, y + 20, 100, 30}, inputBuffer, 64, isInputActive)) isInputActive = !isInputActive;

    if (UIHelper::DrawModernBtn({x + 10, y + 70, 170, 30}, "Delete Word", true, config) || (IsKeyPressed(KEY_ENTER))) {
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
    Color panelBg = UIHelper::GetCanvasBg(config);
    Color outlineCol = UIHelper::GetOutlineCol(config);
    Color textCol = UIHelper::GetTextCol(config);

    DrawRectangleRounded({x, y, 200, 120}, 0.1f, 8, panelBg);
    DrawRectangleRoundedLines({x, y, 200, 120}, 0.1f, 8, outlineCol);

    DrawText("Word:", x + 10, y + 25, 20, textCol);
    if (GuiTextBox((Rectangle){x + 80, y + 20, 100, 30}, inputBuffer, 64, isInputActive)) isInputActive = !isInputActive;

    if (UIHelper::DrawModernBtn({x + 10, y + 70, 170, 30}, "Search Word", true, config) || (IsKeyPressed(KEY_ENTER))) {
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
    Color panelBg = UIHelper::GetCanvasBg(config);
    Color outlineCol = UIHelper::GetOutlineCol(config);
    Color textCol = UIHelper::GetTextCol(config);

    DrawRectangleRounded({x, y, 350, 120}, 0.1f, 8, panelBg);
    DrawRectangleRoundedLines({x, y, 350, 120}, 0.1f, 8, outlineCol);

    DrawText("Word:", x + 10, y + 25, 20, textCol);
    if (GuiTextBox((Rectangle){x + 70, y + 20, 80, 30}, inputBuffer, 64, isInputActive)) isInputActive = !isInputActive;

    DrawText("Size:", x + 170, y + 25, 20, textCol);
    if (GuiTextBox((Rectangle){x + 230, y + 20, 60, 30}, sizeBuffer, 64, isSizeActive)) isSizeActive = !isSizeActive;

    if (UIHelper::DrawModernBtn({x + 10, y + 70, 320, 30}, "Update Word Size", true, config) || (IsKeyPressed(KEY_ENTER))) {
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
    Color textCol = config.isDarkMode ? Color{226, 215, 193, 255} : Color{40, 40, 40, 255};
    Color panelBg = config.isDarkMode ? Color{57, 62, 70, 230} : Color{245, 232, 232, 230};
    Color outlineCol = config.isDarkMode ? Color{162, 151, 137, 255} : Color{238, 217, 217, 255};

    int centerX = GetScreenWidth() / 2;
    int y = GetScreenHeight() - 240;

    // Draw the floating pill background
    Rectangle playbackRect = {(float)centerX - 120, (float)y - 10, 560, 50};
    DrawRectangleRounded(playbackRect, 0.5f, 10, panelBg);
    DrawRectangleRoundedLines(playbackRect, 0.5f, 10, outlineCol);

    // 🌟 HELPER: Mini Modern Button for Playback
    auto DrawMiniBtn = [&](Rectangle rect, const char* text) -> bool {
        Vector2 mouse = GetMousePosition();
        bool isHovering = CheckCollisionPointRec(mouse, rect);

        Color bg = isHovering ? Fade(textCol, 0.15f) : Fade(textCol, 0.05f);
        DrawRectangleRounded(rect, 0.4f, 8, bg);
        DrawRectangleRoundedLines(rect, 0.4f, 8, outlineCol);

        int tw = MeasureText(text, 18);
        DrawText(text, rect.x + rect.width/2 - tw/2, rect.y + rect.height/2 - 9, 18, textCol);

        return isHovering && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    };

    // 🌟 REPLACE GuiButton with DrawMiniBtn!
    if (DrawMiniBtn({(float)centerX - 100, (float)y, 40, 30}, "<<")) Visualizer::Instance().GoToStart();
    if (DrawMiniBtn({(float)centerX - 50, (float)y, 40, 30}, "<")) Visualizer::Instance().PrevStep();

    const char* label = Visualizer::Instance().IsPlaying() ? "||" : ">";
    if (DrawMiniBtn({(float)centerX, (float)y, 40, 30}, label)) Visualizer::Instance().TogglePlay();

    if (DrawMiniBtn({(float)centerX + 50, (float)y, 40, 30}, ">")) Visualizer::Instance().NextStep();
    if (DrawMiniBtn({(float)centerX + 100, (float)y, 40, 30}, ">>")) Visualizer::Instance().GoToEnd();

    // Speed Slider
    UIHelper::DrawModernSlider({(float)centerX + 265, (float)y, 110, 30}, "Fast", "Slow", &playbackSpeed, 0.1f, 2.0f, config);
    Visualizer::Instance().SetSpeed(playbackSpeed);
}

void TrieState::DrawPseudocode() {
    int panelW = 600;
    int panelH = 200;
    int startX = GetScreenWidth() - panelW;
    int startY = GetScreenHeight() - 200;

    // 🌟 USE DYNAMIC THEME COLORS INSTEAD OF BLACK
    Color panelBg = config.isDarkMode ? Color{45, 50, 60, 255} : Color{240, 225, 225, 255};
    Color outlineCol = config.isDarkMode ? Color{162, 151, 137, 255} : Color{238, 217, 217, 255};
    Color textCol = config.isDarkMode ? Color{226, 215, 193, 255} : Color{40, 40, 40, 255};

    DrawRectangle(startX, startY, panelW, panelH, panelBg);
    DrawRectangleLines(startX, startY, panelW, panelH, outlineCol);
    DrawText("Pseudocode", startX + 15, startY + 15, 20, textCol);
    DrawLine(startX, startY + 45, startX + panelW, startY + 45, outlineCol);

    const AnimationState& state = Visualizer::Instance().GetCurrentState();
    if (state.codeText.empty()) return;

    int y = startY + 55;
    for (int i = 0; i < state.codeText.size(); i++) {
        if (i == state.codeLineIndex) {
            // Highlight active line
            DrawRectangle(startX, y - 2, panelW, 25, config.isDarkMode ? Color{80, 85, 95, 255} : Color{220, 190, 190, 255});
            DrawText(state.codeText[i].c_str(), startX + 15, y, 20, textCol);
        } else {
            // Dim inactive lines
            DrawText(state.codeText[i].c_str(), startX + 15, y, 20, Fade(textCol, 0.5f));
        }
        y += 25;
    }
}

void TrieState::DrawSettingsModal() {
    // 🌟 INCREASED SIZE
    float width = 400; float height = 340;
    float startX = GetScreenWidth() / 2 - width / 2;
    float startY = GetScreenHeight() / 2 - height / 2;

    Color modalBg = config.isDarkMode ? Color{57, 62, 70, 255} : Color{251, 239, 239, 255};
    Color textCol = config.isDarkMode ? Color{223, 208, 184, 255} : BLACK;

    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), {0, 0, 0, 150});
    DrawRectangle(startX, startY, width, height, modalBg);
    DrawRectangleLines(startX, startY, width, height, DARKGRAY);

    DrawText("Visual Settings", startX + 120, startY + 20, 22, textCol);
    DrawLine(startX + 20, startY + 55, startX + width - 20, startY + 55, GRAY);

    // 🌟 INCREASED SPACING
    GuiCheckBox((Rectangle){startX + 30, startY + 80, 24, 24}, " Dark Mode", &config.isDarkMode);

    DrawText("Node Radius", startX + 30, startY + 130, 18, textCol);
    GuiSliderBar((Rectangle){startX + 160, startY + 130, 180, 20}, NULL, NULL, &config.nodeRadius, 15.0f, 40.0f);

    DrawText("Edge Width", startX + 30, startY + 180, 18, textCol);
    GuiSliderBar((Rectangle){startX + 160, startY + 180, 180, 20}, NULL, NULL, &config.edgeThickness, 1.0f, 10.0f);

    DrawText("Text Size", startX + 30, startY + 230, 18, textCol);
    GuiSliderBar((Rectangle){startX + 160, startY + 230, 180, 20}, NULL, NULL, &config.textSize, 12.0f, 30.0f);

    if (GuiButton((Rectangle){startX + width / 2 - 50, startY + 285, 100, 35}, "Close")) {
        isSettingsOpen = false;
    }
}


