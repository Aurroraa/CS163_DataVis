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
    Color panelBg = UIHelper::GetCanvasBg(config); Color outlineCol = UIHelper::GetOutlineCol(config); Color textCol = UIHelper::GetTextCol(config);
    DrawRectangleRounded({x, y, 760.0f, 130.0f}, 0.1f, 8, panelBg);
    DrawRectangleRoundedLines({x, y, 760.0f, 130.0f}, 0.1f, 8, outlineCol);
    float fontSize = 24.0f;

    // --- ROW 1 ---
    if (UIHelper::DrawModernBtn({x + 15.0f, y + 15.0f, 190.0f, 40.0f}, "Empty (Clear)", true, config)) {
        Visualizer::Instance().ClearHistory(); trie.init({}); Visualizer::Instance().SetPlaying(false);
    }
    Vector2 lblATw = MeasureTextEx(g_App->mainFont, "Words (csv):", fontSize, 1.0f);
    DrawTextEx(g_App->mainFont, "Words (csv):", {x + 225.0f, y + 15.0f + 20.0f - lblATw.y/2.0f}, fontSize, 1.0f, textCol);

    if (GuiTextBox((Rectangle){x + 400.0f, y + 15.0f, 250.0f, 40.0f}, inputBuffer, 64, isInputActive)) isInputActive = !isInputActive;

    if (UIHelper::DrawModernBtn({x + 670.0f, y + 15.0f, 60.0f, 40.0f}, "Go", true, config) || (IsKeyPressed(KEY_ENTER))) {
        if (inputBuffer[0] != '\0') {
            Visualizer::Instance().ClearHistory(); std::vector<std::string> words; std::stringstream ss(inputBuffer); std::string seg;
            while(std::getline(ss, seg, ',')) words.push_back(seg);
            if (!words.empty()) trie.init(words);
            Visualizer::Instance().SetStep(0); Visualizer::Instance().SetPlaying(true);
            inputBuffer[0] = '\0'; isInputActive = false;
        }
    }

    // --- ROW 2 ---
    Vector2 lblNTw = MeasureTextEx(g_App->mainFont, "Random N =", fontSize, 1.0f);
    DrawTextEx(g_App->mainFont, "Random N =", {x + 15.0f, y + 70.0f + 20.0f - lblNTw.y/2.0f}, fontSize, 1.0f, textCol);

    if (GuiTextBox((Rectangle){x + 165.0f, y + 70.0f, 70.0f, 40.0f}, nBuffer, 16, nInputActive)) nInputActive = !nInputActive;

    if (UIHelper::DrawModernBtn({x + 250.0f, y + 70.0f, 150.0f, 40.0f}, "Generate", true, config) || (IsKeyPressed(KEY_ENTER))) {
        if (nBuffer[0] != '\0') {
            Visualizer::Instance().ClearHistory();
            int n = atoi(nBuffer); if (n < 1) n = 1; if (n > 10) n = 10;
            std::vector<std::string> words;
            for(int i = 0; i < n; i++) {
                int wordLength = GetRandomValue(3, 6); std::string randomWord = "";
                for(int j = 0; j < wordLength; j++) randomWord += (char)GetRandomValue('a', 'z');
                words.push_back(randomWord);
            }
            trie.init(words); Visualizer::Instance().SetStep(0); Visualizer::Instance().SetPlaying(true);
            nBuffer[0] = '\0'; nInputActive = false;
        }
    }

    if (UIHelper::DrawModernBtn({x + 415.0f, y + 70.0f, 220.0f, 40.0f}, "Browse File...", true, config)) {
        const char *filterPatterns[1] = { "*.txt" }; const char *filePath = tinyfd_openFileDialog("Select Input File", "", 1, filterPatterns, "Text Files", 0);
        if (filePath != NULL) {
            std::ifstream file(filePath);
            if (file.is_open()) {
                std::vector<std::string> words; std::string word;
                while (file >> word) {
                    word.erase(std::remove(word.begin(), word.end(), ','), word.end());
                    if (!word.empty()) words.push_back(word);
                }
                file.close(); if (!words.empty()) trie.init(words);
                Visualizer::Instance().SetStep(0); Visualizer::Instance().SetPlaying(true);
            }
        }
    }
}

void TrieState::DrawAddMenu(float x, float y) {
    Color panelBg = UIHelper::GetCanvasBg(config); Color outlineCol = UIHelper::GetOutlineCol(config); Color textCol = UIHelper::GetTextCol(config);
    DrawRectangleRounded({x, y, 320.0f, 130.0f}, 0.1f, 8, panelBg);
    DrawRectangleRoundedLines({x, y, 320.0f, 130.0f}, 0.1f, 8, outlineCol);
    float fontSize = 24.0f;

    Vector2 valTw = MeasureTextEx(g_App->mainFont, "Word:", fontSize, 1.0f);
    DrawTextEx(g_App->mainFont, "Word:", {x + 15.0f, y + 15.0f + 20.0f - valTw.y/2.0f}, fontSize, 1.0f, textCol);
    if (GuiTextBox((Rectangle){x + 95.0f, y + 15.0f, 200.0f, 40.0f}, inputBuffer, 64, isInputActive)) isInputActive = !isInputActive;

    if (UIHelper::DrawModernBtn({x + 15.0f, y + 70.0f, 280.0f, 40.0f}, "Insert Word", true, config) || IsKeyPressed(KEY_ENTER)) {
        if (inputBuffer[0] != '\0') {
            Visualizer::Instance().ClearHistory(); Visualizer::Instance().RecordState("Initial State", 0, trie.captureState(), {});
            trie.insert(inputBuffer); Visualizer::Instance().SetStep(0); Visualizer::Instance().SetPlaying(true);
            inputBuffer[0] = '\0'; isInputActive = false;
        }
    }
}

void TrieState::DrawDeleteMenu(float x, float y) {
    Color panelBg = UIHelper::GetCanvasBg(config); Color outlineCol = UIHelper::GetOutlineCol(config); Color textCol = UIHelper::GetTextCol(config);
    DrawRectangleRounded({x, y, 320.0f, 130.0f}, 0.1f, 8, panelBg);
    DrawRectangleRoundedLines({x, y, 320.0f, 130.0f}, 0.1f, 8, outlineCol);
    float fontSize = 24.0f;

    Vector2 valTw = MeasureTextEx(g_App->mainFont, "Word:", fontSize, 1.0f);
    DrawTextEx(g_App->mainFont, "Word:", {x + 15.0f, y + 15.0f + 20.0f - valTw.y/2.0f}, fontSize, 1.0f, textCol);
    if (GuiTextBox((Rectangle){x + 95.0f, y + 15.0f, 200.0f, 40.0f}, inputBuffer, 64, isInputActive)) isInputActive = !isInputActive;

    if (UIHelper::DrawModernBtn({x + 15.0f, y + 70.0f, 280.0f, 40.0f}, "Delete Word", true, config) || (IsKeyPressed(KEY_ENTER))) {
        if (inputBuffer[0] != '\0') {
            Visualizer::Instance().ClearHistory(); Visualizer::Instance().RecordState("Initial State", 0, trie.captureState(), {});
            trie.deleteWord(inputBuffer); Visualizer::Instance().SetStep(0); Visualizer::Instance().SetPlaying(true);
            inputBuffer[0] = '\0'; isInputActive = false;
        }
    }
}

void TrieState::DrawSearchMenu(float x, float y) {
    Color panelBg = UIHelper::GetCanvasBg(config); Color outlineCol = UIHelper::GetOutlineCol(config); Color textCol = UIHelper::GetTextCol(config);
    DrawRectangleRounded({x, y, 320.0f, 130.0f}, 0.1f, 8, panelBg);
    DrawRectangleRoundedLines({x, y, 320.0f, 130.0f}, 0.1f, 8, outlineCol);
    float fontSize = 24.0f;

    Vector2 valTw = MeasureTextEx(g_App->mainFont, "Word:", fontSize, 1.0f);
    DrawTextEx(g_App->mainFont, "Word:", {x + 15.0f, y + 15.0f + 20.0f - valTw.y/2.0f}, fontSize, 1.0f, textCol);
    if (GuiTextBox((Rectangle){x + 95.0f, y + 15.0f, 200.0f, 40.0f}, inputBuffer, 64, isInputActive)) isInputActive = !isInputActive;

    if (UIHelper::DrawModernBtn({x + 15.0f, y + 70.0f, 280.0f, 40.0f}, "Search Word", true, config) || (IsKeyPressed(KEY_ENTER))) {
        if (inputBuffer[0] != '\0') {
            Visualizer::Instance().ClearHistory(); Visualizer::Instance().RecordState("Initial State", 0, trie.captureState(), {});
            trie.searchWord(inputBuffer); Visualizer::Instance().SetStep(0); Visualizer::Instance().SetPlaying(true);
            inputBuffer[0] = '\0'; isInputActive = false;
        }
    }
}

void TrieState::DrawUpdateMenu(float x, float y) {
    Color panelBg = UIHelper::GetCanvasBg(config); Color outlineCol = UIHelper::GetOutlineCol(config); Color textCol = UIHelper::GetTextCol(config);
    DrawRectangleRounded({x, y, 500.0f, 130.0f}, 0.1f, 8, panelBg);
    DrawRectangleRoundedLines({x, y, 500.0f, 130.0f}, 0.1f, 8, outlineCol);
    float fontSize = 24.0f;

    Vector2 valTw = MeasureTextEx(g_App->mainFont, "Word:", fontSize, 1.0f);
    DrawTextEx(g_App->mainFont, "Word:", {x + 15.0f, y + 15.0f + 20.0f - valTw.y/2.0f}, fontSize, 1.0f, textCol);
    if (GuiTextBox((Rectangle){x + 95.0f, y + 15.0f, 120.0f, 40.0f}, inputBuffer, 64, isInputActive)) isInputActive = !isInputActive;

    Vector2 sizeTw = MeasureTextEx(g_App->mainFont, "Size:", fontSize, 1.0f);
    DrawTextEx(g_App->mainFont, "Size:", {x + 235.0f, y + 15.0f + 20.0f - sizeTw.y/2.0f}, fontSize, 1.0f, textCol);
    if (GuiTextBox((Rectangle){x + 315.0f, y + 15.0f, 100.0f, 40.0f}, sizeBuffer, 64, isSizeActive)) isSizeActive = !isSizeActive;

    if (UIHelper::DrawModernBtn({x + 15.0f, y + 70.0f, 400.0f, 40.0f}, "Update Word Size", true, config) || (IsKeyPressed(KEY_ENTER))) {
        if (inputBuffer[0] != '\0' && sizeBuffer[0] != '\0') {
            Visualizer::Instance().ClearHistory(); Visualizer::Instance().RecordState("Initial State", 0, trie.captureState(), {});
            trie.updateWord(inputBuffer, atoi(sizeBuffer));
            Visualizer::Instance().SetStep(0); Visualizer::Instance().SetPlaying(true);
            inputBuffer[0] = '\0'; sizeBuffer[0] = '\0'; isInputActive = false; isSizeActive = false;
        }
    }
}

void TrieState::DrawPlayback() {
    Color textCol = config.isDarkMode ? Color{226, 215, 193, 255} : Color{40, 40, 40, 255};
    Color panelBg = config.isDarkMode ? Color{57, 62, 70, 230} : Color{245, 232, 232, 230};
    Color outlineCol = config.isDarkMode ? Color{162, 151, 137, 255} : Color{238, 217, 217, 255};

    float centerX = GetScreenWidth() / 2.0f;
    float y = GetScreenHeight() - 240.0f;

    Rectangle playbackRect = {centerX - 120.0f, y - 10.0f, 560.0f, 50.0f};
    DrawRectangleRounded(playbackRect, 0.5f, 10, panelBg);
    DrawRectangleRoundedLines(playbackRect, 0.5f, 10, outlineCol);

    auto DrawMiniBtn = [&](Rectangle rect, const char* text) -> bool {
        Vector2 mouse = GetMousePosition();
        bool isHovering = CheckCollisionPointRec(mouse, rect);

        Color bg = isHovering ? Fade(textCol, 0.15f) : Fade(textCol, 0.05f);
        DrawRectangleRounded(rect, 0.4f, 8, bg);
        DrawRectangleRoundedLines(rect, 0.4f, 8, outlineCol);

        // 🌟 PERFECT CENTERING WITH 24px FONT
        float fontSize = 24.0f;
        Vector2 tw = MeasureTextEx(g_App->mainFont, text, fontSize, 1.0f);
        DrawTextEx(g_App->mainFont, text,
            {rect.x + rect.width/2.0f - tw.x/2.0f, rect.y + rect.height/2.0f - tw.y/2.0f},
            fontSize, 1.0f, textCol);

        return isHovering && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    };

    if (DrawMiniBtn({centerX - 100.0f, y, 40.0f, 30.0f}, "<<")) Visualizer::Instance().GoToStart();
    if (DrawMiniBtn({centerX - 50.0f, y, 40.0f, 30.0f}, "<")) Visualizer::Instance().PrevStep();

    const char* label = Visualizer::Instance().IsPlaying() ? "||" : ">";
    if (DrawMiniBtn({centerX, y, 40.0f, 30.0f}, label)) Visualizer::Instance().TogglePlay();

    if (DrawMiniBtn({centerX + 50.0f, y, 40.0f, 30.0f}, ">")) Visualizer::Instance().NextStep();
    if (DrawMiniBtn({centerX + 100.0f, y, 40.0f, 30.0f}, ">>")) Visualizer::Instance().GoToEnd();

    UIHelper::DrawModernSlider({centerX + 265.0f, y, 110.0f, 30.0f}, "Fast", "Slow", &playbackSpeed, 0.1f, 2.0f, config);
    Visualizer::Instance().SetSpeed(playbackSpeed);
}

void TrieState::DrawPseudocode() {
    float panelW = 600.0f;
    float panelH = 200.0f;
    float startX = GetScreenWidth() - panelW;
    float startY = GetScreenHeight() - panelW/3.0f; // Ensure scaling aligns

    Color panelBg = config.isDarkMode ? Color{45, 50, 60, 255} : Color{240, 225, 225, 255};
    Color outlineCol = config.isDarkMode ? Color{162, 151, 137, 255} : Color{238, 217, 217, 255};
    Color textCol = config.isDarkMode ? Color{226, 215, 193, 255} : Color{40, 40, 40, 255};

    DrawRectangle(startX, startY, panelW, panelH, panelBg);
    DrawRectangleLines(startX, startY, panelW, panelH, outlineCol);
    DrawTextEx(g_App->boldFont, "Pseudocode", {startX + 15.0f, startY + 15.0f}, 24.0f, 1.0f, textCol);
    DrawLine(startX, startY + 45.0f, startX + panelW, startY + 45.0f, outlineCol);

    const AnimationState& state = Visualizer::Instance().GetCurrentState();
    if (state.codeText.empty()) return;

    float y = startY + 55.0f;
    for (int i = 0; i < state.codeText.size(); i++) {
        if (i == state.codeLineIndex) {
            DrawRectangle(startX, y - 2.0f, panelW, 25.0f, config.isDarkMode ? Color{80, 85, 95, 255} : Color{220, 190, 190, 255});
            DrawTextEx(g_App->mainFont, state.codeText[i].c_str(), {startX + 15.0f, y}, 24.0f, 1.0f, textCol);
        } else {
            DrawTextEx(g_App->mainFont, state.codeText[i].c_str(), {startX + 15.0f, y}, 20.0f, 1.0f, Fade(textCol, 0.5f));
        }
        y += 25.0f;
    }
}

void TrieState::DrawSettingsModal() {
    float width = 400.0f; float height = 340.0f;
    float startX = GetScreenWidth() / 2.0f - width / 2.0f;
    float startY = GetScreenHeight() / 2.0f - height / 2.0f;

    Color modalBg = config.isDarkMode ? Color{57, 62, 70, 255} : Color{251, 239, 239, 255};
    Color textCol = config.isDarkMode ? Color{223, 208, 184, 255} : BLACK;

    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), {0, 0, 0, 150});
    DrawRectangle(startX, startY, width, height, modalBg);
    DrawRectangleLines(startX, startY, width, height, DARKGRAY);

    Vector2 ts = MeasureTextEx(g_App->boldFont, "Visual Settings", 26.0f, 1.0f);
    DrawTextEx(g_App->boldFont, "Visual Settings", {startX + width/2.0f - ts.x/2.0f, startY + 20.0f}, 26.0f, 1.0f, textCol);
    DrawLine(startX + 20.0f, startY + 55.0f, startX + width - 20.0f, startY + 55.0f, GRAY);

    GuiCheckBox((Rectangle){startX + 30.0f, startY + 80.0f, 24.0f, 24.0f}, " Dark Mode", &config.isDarkMode);

    // 🌟 DYNAMICALLY CENTERED LABELS FOR 20px SLIDERS
    float lblSize = 22.0f;

    Vector2 radTw = MeasureTextEx(g_App->mainFont, "Node Radius", lblSize, 1.0f);
    DrawTextEx(g_App->mainFont, "Node Radius", {startX + 30.0f, (startY + 130.0f + 10.0f) - (radTw.y/2.0f)}, lblSize, 1.0f, textCol);
    GuiSliderBar((Rectangle){startX + 160.0f, startY + 130.0f, 180.0f, 20.0f}, NULL, NULL, &config.nodeRadius, 15.0f, 40.0f);

    Vector2 edgeTw = MeasureTextEx(g_App->mainFont, "Edge Width", lblSize, 1.0f);
    DrawTextEx(g_App->mainFont, "Edge Width", {startX + 30.0f, (startY + 180.0f + 10.0f) - (edgeTw.y/2.0f)}, lblSize, 1.0f, textCol);
    GuiSliderBar((Rectangle){startX + 160.0f, startY + 180.0f, 180.0f, 20.0f}, NULL, NULL, &config.edgeThickness, 1.0f, 10.0f);

    Vector2 txtTw = MeasureTextEx(g_App->mainFont, "Text Size", lblSize, 1.0f);
    DrawTextEx(g_App->mainFont, "Text Size", {startX + 30.0f, (startY + 230.0f + 10.0f) - (txtTw.y/2.0f)}, lblSize, 1.0f, textCol);
    GuiSliderBar((Rectangle){startX + 160.0f, startY + 230.0f, 180.0f, 20.0f}, NULL, NULL, &config.textSize, 12.0f, 30.0f);

    if (GuiButton((Rectangle){startX + width / 2.0f - 50.0f, startY + 285.0f, 100.0f, 35.0f}, "Close")) {
        isSettingsOpen = false;
    }
}

