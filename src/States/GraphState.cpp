#include "States/GraphState.h"
#include "States/SelectState.h"
#include "App.h"
#include "raygui.h"
#include "Visualizer.h"
#include "Renderers/GraphRenderer.h"
#include <algorithm>
#include <cstring>
#include "../../include/tinyfiledialogs.h"
#include <fstream>

GraphState::GraphState() {
    Visualizer::Instance().ClearHistory();
    graph.clear();

    draggingVertex = -1;
    isDirected = false;
}

void GraphState::Init() {
    textEditorBuffer[0] = '\0';
    lastText = "";
    isEditorActive = false;
    draggingVertex = -1;
}

void GraphState::Update() {
    Visualizer::Instance().Update();

    float canvasStartX = 260.0f;
    float canvasStartY = 0.0f;
    float canvasWidth = GetScreenWidth() - 260.0f;
    float canvasHeight = GetScreenHeight() - 200.0f;

    // 1. LIVE PARSER DETECTOR
    std::string currentText(textEditorBuffer);
    if (currentText != lastText) {
        lastText = currentText;
        isAlgorithmActive = false; // 🌟 Editing text returns to Sandbox Mode!
        Visualizer::Instance().ClearHistory();
        graph.initFromLiveText(currentText, isDirected, canvasStartX, canvasStartY, canvasWidth, canvasHeight);
    }

    // 2. PHYSICS & DRAGGING ENGINE
    // 🌟 ONLY run physics if we are NOT looking at an algorithm!
    if (!isAlgorithmActive) {
        Vector2 mousePos = GetMousePosition();
        bool isHoveringCanvas = (mousePos.x > canvasStartX && mousePos.y < canvasHeight);

        static Vector2 dragStartPos = {0, 0};
        static bool isDraggingNode = false;

        if (isHoveringCanvas) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                draggingVertex = graph.getClickedVertex(mousePos.x, mousePos.y);
                if (draggingVertex != -1) {
                    dragStartPos = mousePos;
                    isDraggingNode = false;
                }
            }

            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && draggingVertex != -1) {
                float dx = mousePos.x - dragStartPos.x;
                float dy = mousePos.y - dragStartPos.y;
                if ((dx*dx + dy*dy) > 25.0f) isDraggingNode = true;

                if (isDraggingNode) {
                    graph.updateVertexPosition(draggingVertex, mousePos.x, mousePos.y,
                                               canvasStartX + 30.0f, GetScreenWidth() - 30.0f,
                                               30.0f, canvasHeight - 30.0f);
                }
            }

            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                if (draggingVertex != -1 && !isDraggingNode) graph.toggleFixed(draggingVertex);
                draggingVertex = -1;
            }
        }

        graph.updatePhysics(canvasStartX, canvasStartY, canvasStartX + canvasWidth, canvasStartY + canvasHeight);

        Visualizer::Instance().ClearHistory();
        Visualizer::Instance().RecordState("", 0, graph.captureState(), {});
    }
    else {
        // 🌟 If Algorithm is active, clicking the canvas cancels it and returns to Sandbox!
        Vector2 mousePos = GetMousePosition();
        if ((mousePos.x > canvasStartX && mousePos.y < canvasHeight) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            isAlgorithmActive = false;
            Visualizer::Instance().SetPlaying(false);
        }
    }
}

void GraphState::Draw() {
    float canvasStartX = 260.0f;
    float canvasHeight = GetScreenHeight() - 200.0f;

    // --- 🌟 EXTRACT EXACT PALETTE COLORS ---
    Color canvasBg = config.isDarkMode ? Color{35, 41, 49, 255} : Color{250, 250, 250, 255};
    Color panelBg = config.isDarkMode ? Color{57, 62, 70, 255} : Color{245, 232, 232, 255};
    Color textCol = config.isDarkMode ? Color{226, 215, 193, 255} : Color{40, 40, 40, 255};
    Color outlineCol = config.isDarkMode ? Color{162, 151, 137, 255} : Color{238, 217, 217, 255};

    DrawRectangle(canvasStartX, 0, GetScreenWidth() - canvasStartX, canvasHeight, canvasBg);
    Visualizer::Instance().DrawCanvas();

    DrawRectangle(0, 0, 260, canvasHeight, panelBg);
    DrawLine(260, 0, 260, canvasHeight, outlineCol);

    // 🌟 BIGGER DIRECTED GRAPH CHECKBOX (24x24 instead of 20x20, and shifted down)
    bool prevDirected = isDirected;
    GuiCheckBox((Rectangle){15, 15, 24, 24}, " Directed Graph", &isDirected);
    if (isDirected != prevDirected) {
        Visualizer::Instance().ClearHistory();
        graph.initFromLiveText(std::string(textEditorBuffer), isDirected, canvasStartX, 0, GetScreenWidth()-canvasStartX, canvasHeight);
    }

    DrawTextEx(g_App->boldFont, "Graph Data:", {10.0f, 45.0f}, 21.0f, 1.0f, textCol);

    // 🌟 ADVANCED MULTI-LINE TEXT BOX WITH CURSOR & CLIPBOARD
    Rectangle textBoxRect = { 0, 70, 260, canvasHeight - 70.0f };
    Vector2 mousePos = GetMousePosition();
    bool clicked = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

    static int cursorPos = 0;
    static bool selectAll = false;
    int textLen = strlen(textEditorBuffer);

    // Safety bounds
    if (cursorPos > textLen) cursorPos = textLen;
    if (cursorPos < 0) cursorPos = 0;

    // 1. Handle Mouse Click Focus
    if (clicked) {
        isEditorActive = CheckCollisionPointRec(mousePos, textBoxRect);
    }

    // 2. Handle Keyboard Input & Clipboard
    if (isEditorActive) {
        bool ctrlDown = IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL);

        if (ctrlDown) {
            if (IsKeyPressed(KEY_A)) selectAll = true;
            if (IsKeyPressed(KEY_C) || IsKeyPressed(KEY_X)) {
                SetClipboardText(textEditorBuffer); // OS Copy
                if (IsKeyPressed(KEY_X)) { textEditorBuffer[0] = '\0'; cursorPos = 0; textLen = 0; selectAll = false; }
            }
            if (IsKeyPressed(KEY_V)) {
                const char* clip = GetClipboardText(); // OS Paste
                if (clip) {
                    if (selectAll) { textEditorBuffer[0] = '\0'; cursorPos = 0; textLen = 0; selectAll = false; }
                    int clipLen = strlen(clip);
                    if (textLen + clipLen < 2047) {
                        memmove(&textEditorBuffer[cursorPos + clipLen], &textEditorBuffer[cursorPos], textLen - cursorPos + 1);
                        memcpy(&textEditorBuffer[cursorPos], clip, clipLen);
                        cursorPos += clipLen;
                    }
                }
            }
        } else {
            // Typing Letters
            int key = GetCharPressed();
            while (key > 0) {
                if ((key >= 32) && (key <= 125) && (textLen < 2047)) {
                    if (selectAll) { textEditorBuffer[0] = '\0'; cursorPos = 0; textLen = 0; selectAll = false; }
                    memmove(&textEditorBuffer[cursorPos + 1], &textEditorBuffer[cursorPos], textLen - cursorPos + 1);
                    textEditorBuffer[cursorPos] = (char)key;
                    cursorPos++; textLen++;
                }
                key = GetCharPressed();
            }

            // Enter Key
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) {
                if (selectAll) { textEditorBuffer[0] = '\0'; cursorPos = 0; textLen = 0; selectAll = false; }
                if (textLen < 2047) {
                    memmove(&textEditorBuffer[cursorPos + 1], &textEditorBuffer[cursorPos], textLen - cursorPos + 1);
                    textEditorBuffer[cursorPos] = '\n';
                    cursorPos++; textLen++;
                }
            }

            // Backspace Key
            if (IsKeyPressed(KEY_BACKSPACE)) {
                if (selectAll) {
                    textEditorBuffer[0] = '\0'; cursorPos = 0; selectAll = false;
                } else if (cursorPos > 0) {
                    memmove(&textEditorBuffer[cursorPos - 1], &textEditorBuffer[cursorPos], textLen - cursorPos + 1);
                    cursorPos--; textLen--;
                }
            }

            // Arrow Keys Navigation
            if (IsKeyPressed(KEY_LEFT) && cursorPos > 0) { cursorPos--; selectAll = false; }
            if (IsKeyPressed(KEY_RIGHT) && cursorPos < textLen) { cursorPos++; selectAll = false; }
        }
    }

    // --- DRAWING & MOUSE MAPPING ---
    Color editorBg = config.isDarkMode ? Color{45, 50, 60, 255} : WHITE;
    Color editorSelectBg = config.isDarkMode ? Color{70, 90, 120, 255} : Color{220, 235, 255, 255};
    Color gutterBg = config.isDarkMode ? Color{35, 41, 49, 255} : Color{230, 230, 230, 255};
    Color gutterText = config.isDarkMode ? Fade(textCol, 0.5f) : DARKGRAY;
    Color activeLineBg = config.isDarkMode ? Color{65, 70, 80, 255} : Color{240, 245, 255, 255};

    // 🌟 2. APPLY THE BACKGROUNDS
    DrawRectangleRec(textBoxRect, selectAll ? editorSelectBg : editorBg);
    Rectangle gutterRect = {textBoxRect.x, textBoxRect.y, 40, textBoxRect.height};
    DrawRectangleRec(gutterRect, gutterBg);

    // Parse lines to draw them individually
    std::vector<std::string> lines;
    std::string currentLineText = "";
    for (int i = 0; i < strlen(textEditorBuffer); i++) {
        if (textEditorBuffer[i] == '\n') {
            lines.push_back(currentLineText); currentLineText = "";
        } else {
            currentLineText += textEditorBuffer[i];
        }
    }
    lines.push_back(currentLineText);

    int charCounter = 0;
    int cursorPixelX = gutterRect.width + 10;
    int cursorPixelY = textBoxRect.y;

    for (int i = 0; i < lines.size(); i++) {
        float lineY = textBoxRect.y + i * 22.0f;

        std::string lineNum = std::to_string(i + 1);
        Vector2 numWidth = MeasureTextEx(g_App->mainFont, lineNum.c_str(), 18.0f, 1.0f);
        DrawTextEx(g_App->mainFont, lineNum.c_str(), {gutterRect.x + (gutterRect.width - numWidth.x) / 2.0f, lineY + 2.0f}, 18.0f, 1.0f, DARKGRAY);

        if (!selectAll && isEditorActive && cursorPos >= charCounter && cursorPos <= charCounter + lines[i].length()) {
            Color activeLineBg = config.isDarkMode ? Color{65, 70, 80, 255} : Color{240, 245, 255, 255};
            DrawRectangle(gutterRect.width, lineY, textBoxRect.width - gutterRect.width, 22.0f, activeLineBg);
        }

        DrawTextEx(g_App->mainFont, lines[i].c_str(), {gutterRect.width + 10.0f, lineY + 2.0f}, 18.0f, 1.0f, textCol);

        if (clicked && isEditorActive && CheckCollisionPointRec(mousePos, textBoxRect)) {
            if (mousePos.y >= lineY && mousePos.y < lineY + 22.0f) {
                float clickLocalX = mousePos.x - (gutterRect.width + 10.0f);
                if (clickLocalX < 0) clickLocalX = 0;

                int bestIdx = 0;
                for(int c = 0; c <= lines[i].length(); c++) {
                    float w = MeasureTextEx(g_App->mainFont, lines[i].substr(0, c).c_str(), 18.0f, 1.0f).x;
                    if (w >= clickLocalX - 5.0f) { bestIdx = c; break; }
                    bestIdx = c;
                }
                cursorPos = charCounter + bestIdx;
                selectAll = false;
            }
        }

        // 🌟 PIXEL PERFECT CURSOR POSITION
        if (cursorPos >= charCounter && cursorPos <= charCounter + lines[i].length()) {
            std::string subStr = lines[i].substr(0, cursorPos - charCounter);
            cursorPixelX = gutterRect.width + 10.0f + MeasureTextEx(g_App->mainFont, subStr.c_str(), 18.0f, 1.0f).x;
            cursorPixelY = lineY;
        }
        charCounter += lines[i].length() + 1;
    }

    // Handle clicking below the last line of text
    if (clicked && isEditorActive && mousePos.y >= textBoxRect.y + lines.size() * 22) {
        cursorPos = textLen;
        selectAll = false;
    }

    // Draw Blinking Cursor exactly where it belongs!
    if (isEditorActive && !selectAll && (int)(GetTime() * 2) % 2 == 0) {
        DrawLine(cursorPixelX, cursorPixelY + 2, cursorPixelX, cursorPixelY + 20, textCol);
    }

    // ... inside GraphState::Draw(), right before the Home button logic ...

    // 🌟 CUSTOM EYE-CATCHING HOME BUTTON (BUMPED TO 24px AND PERFECTLY CENTERED)
    float btnX = GetScreenWidth() - 150.0f;
    float homeBtnX = btnX - 110.0f;
    Rectangle homeRect = {homeBtnX, 10.0f, 90.0f, 36.0f};
    bool hoverHome = CheckCollisionPointRec(mousePos, homeRect);

    // Draw rounded background (Accent color if hovered)
    Color homeBg = hoverHome ? (config.isDarkMode ? Color{162, 151, 137, 255} : Color{242, 182, 182, 255}) : Fade(textCol, 0.1f);
    DrawRectangleRounded(homeRect, 0.5f, 8, homeBg);
    DrawRectangleRoundedLines(homeRect, 0.5f, 8, textCol);

    float homeFontSize = 24.0f;
    Vector2 textW = MeasureTextEx(g_App->mainFont, "Home", homeFontSize, 1.0f);
    // 🌟 Perfect Y-centering formula applied!
    DrawTextEx(g_App->mainFont, "Home",
        {homeRect.x + homeRect.width/2.0f - textW.x/2.0f, homeRect.y + homeRect.height/2.0f - textW.y/2.0f},
        homeFontSize, 1.0f, hoverHome ? (config.isDarkMode ? BLACK : WHITE) : textCol);

    if (hoverHome && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        g_App->ChangeState(new SelectState());
    }

    GraphRenderer::Draw(Visualizer::Instance().GetRenderState(), config);
    DrawToolbar();
    DrawPseudocode();
    DrawPlayback();

    // Hamburger Menu
    btnX = GetScreenWidth() - 150.0f;
    Rectangle burgerRect = {btnX, 10.0f, 40.0f, 30.0f};
    mousePos = GetMousePosition();
    bool isHoveringBurger = CheckCollisionPointRec(mousePos, burgerRect);

    if (isHoveringBurger && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        isSettingsOpen = !isSettingsOpen;
    }

    if (isHoveringBurger) {
        DrawRectangleRounded(burgerRect, 0.2f, 4, config.isDarkMode ? Color{80, 85, 95, 255} : Color{220, 220, 220, 255});
    }

    DrawRectangleRounded({btnX + 8.0f, 14.0f, 24.0f, 4.0f}, 1.0f, 4, textCol);
    DrawRectangleRounded({btnX + 8.0f, 23.0f, 24.0f, 4.0f}, 1.0f, 4, textCol);
    DrawRectangleRounded({btnX + 8.0f, 32.0f, 24.0f, 4.0f}, 1.0f, 4, textCol);

    if (isSettingsOpen) DrawSettingsModal();
}

void GraphState::DrawToolbar() {
    Color panelBg = config.isDarkMode ? Color{57, 62, 70, 255} : Color{245, 232, 232, 255};
    Color outlineCol = config.isDarkMode ? Color{162, 151, 137, 255} : Color{238, 217, 217, 255};
    Color textCol = config.isDarkMode ? Color{226, 215, 193, 255} : Color{40, 40, 40, 255};

    Color primaryBg = config.isDarkMode ? Color{162, 151, 137, 255} : Color{242, 182, 182, 255};

    float startY = GetScreenHeight() - 200.0f;
    DrawRectangle(0, startY, GetScreenWidth(), 200.0f, panelBg);
    DrawLine(0, startY, GetScreenWidth(), startY, outlineCol);

    auto DrawModernBtn = [&](Rectangle rect, const char* text, bool isPrimary) -> bool {
        Vector2 mouse = GetMousePosition();
        bool isHovering = CheckCollisionPointRec(mouse, rect);

        Color bg = Fade(textCol, 0.05f);
        if (isPrimary) bg = isHovering ? Fade(primaryBg, 0.8f) : primaryBg;
        else if (isHovering) bg = Fade(textCol, 0.15f);

        DrawRectangleRounded(rect, 0.4f, 8, bg);
        DrawRectangleRoundedLines(rect, 0.4f, 8, isPrimary ? bg : outlineCol);

        Color labelCol = isPrimary ? (config.isDarkMode ? BLACK : WHITE) : textCol;

        // 🌟 BUMPED TO 24px!
        float fontSize = 24.0f;
        Vector2 tw = MeasureTextEx(g_App->mainFont, text, fontSize, 1.0f);
        DrawTextEx(g_App->mainFont, text,
            {rect.x + rect.width/2.0f - tw.x/2.0f, rect.y + rect.height/2.0f - tw.y/2.0f},
            fontSize, 1.0f, labelCol);

        return isHovering && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    };

    float x = 30.0f; float y = startY + 25.0f;

    // --- 🚀 ALGORITHM GROUP (LEFT SIDE) ---
    DrawTextEx(g_App->boldFont, "Algorithms", {x, y}, 24.0f, 1.0f, textCol);
    DrawLine(x, y + 28.0f, x + 250.0f, y + 28.0f, outlineCol);

    if (DrawModernBtn({x, y + 40.0f, 250.0f, 40.0f}, "Run Kruskal (MST)", true)) {
        isAlgorithmActive = true;
        Visualizer::Instance().ClearHistory();
        graph.runKruskalMST();
        Visualizer::Instance().SetStep(0);
        Visualizer::Instance().SetPlaying(true);
    }

    // Centered label alignment with textbox
    Vector2 srcTw = MeasureTextEx(g_App->mainFont, "Src:", 24.0f, 1.0f);
    float labelY = (y + 95.0f + 20.0f) - (srcTw.y / 2.0f);
    DrawTextEx(g_App->mainFont, "Src:", {x + 5.0f, labelY}, 24.0f, 1.0f, textCol);

    if (GuiTextBox((Rectangle){x + 65.0f, y + 95.0f, 40.0f, 40.0f}, dijkstraSourceBuffer, 16, isDijkstraSourceActive)) {
        isDijkstraSourceActive = !isDijkstraSourceActive;
    }

    if (DrawModernBtn({x + 115.0f, y + 95.0f, 160.0f, 40.0f}, "Run Dijkstra", true)) {
        int startNode = -1;
        if (dijkstraSourceBuffer[0] != '\0') {
            try { startNode = std::stoi(dijkstraSourceBuffer); } catch (...) {}
        } else startNode = graph.getFirstVertexId();

        if (startNode != -1) {
            isAlgorithmActive = true;
            Visualizer::Instance().ClearHistory();
            graph.runDijkstra(startNode);
            Visualizer::Instance().SetStep(0);
            Visualizer::Instance().SetPlaying(true);
        }
    }

    // --- 🛠️ UTILITY GROUP (RIGHT SIDE) ---
    float rx = 320.0f;
    DrawTextEx(g_App->boldFont, "Sandbox Tools", {rx, y}, 24.0f, 1.0f, textCol);
    DrawLine(rx, y + 28.0f, rx + 400.0f, y + 28.0f, outlineCol);

    if (DrawModernBtn({rx, y + 40.0f, 120.0f, 40.0f}, "Random", false)) {
        std::string randomGraph = "";
        int numNodes = GetRandomValue(4, 7);
        int numEdges = GetRandomValue(numNodes, numNodes * 2);
        for (int i = 0; i < numEdges; i++) {
            int u = GetRandomValue(1, numNodes); int v = GetRandomValue(1, numNodes);
            while (u == v) v = GetRandomValue(1, numNodes);
            randomGraph += std::to_string(u) + " " + std::to_string(v) + " " + std::to_string(GetRandomValue(1, 99)) + "\n";
        }
        strncpy(textEditorBuffer, randomGraph.c_str(), 2047);
        textEditorBuffer[2047] = '\0';
    }

    if (DrawModernBtn({rx + 130.0f, y + 40.0f, 120.0f, 40.0f}, "Load File", false)) {
        const char *filterPatterns[1] = { "*.txt" };
        const char *filePath = tinyfd_openFileDialog("Select Edge List File", "", 1, filterPatterns, "Text Files", 0);
        if (filePath != NULL) {
            std::ifstream file(filePath);
            if (file.is_open()) {
                std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                strncpy(textEditorBuffer, content.c_str(), 2047);
                textEditorBuffer[2047] = '\0';
                file.close();
            }
        }
    }

    if (DrawModernBtn({rx, y + 95.0f, 250.0f, 40.0f}, "Clear Screen", false)) {
        isAlgorithmActive = false;
        Visualizer::Instance().ClearHistory();
        graph.clear();
        textEditorBuffer[0] = '\0';
    }
}

void GraphState::DrawPlayback() {
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

        // 🌟 BUMPED TO 24px AND PERFECTLY CENTERED
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

void GraphState::DrawPseudocode() {
    const AnimationState& state = Visualizer::Instance().GetCurrentState();

    float panelW = 600.0f;
    float lineSpacing = 28.0f;
    float headerSpace = 55.0f;
    float bottomPadding = 15.0f;

    // 🌟 DYNAMIC HEIGHT CALCULATION
    int numLines = state.codeText.empty() ? 1 : state.codeText.size();
    float calculatedH = headerSpace + (numLines * lineSpacing) + bottomPadding;
    float panelH = std::max(200.0f, calculatedH); // Ensure it's at least 200px to match the toolbar!

    float startX = GetScreenWidth() - panelW;
    float startY = GetScreenHeight() - panelH; // Expands upwards!

    Color panelBg = config.isDarkMode ? Color{45, 50, 60, 255} : Color{240, 225, 225, 255};
    Color outlineCol = config.isDarkMode ? Color{162, 151, 137, 255} : Color{238, 217, 217, 255};
    Color textCol = config.isDarkMode ? Color{226, 215, 193, 255} : Color{40, 40, 40, 255};

    DrawRectangle(startX, startY, panelW, panelH, panelBg);
    DrawRectangleLines(startX, startY, panelW, panelH, outlineCol);

    // Header
    DrawTextEx(g_App->boldFont, "Pseudocode", {startX + 15.0f, startY + 15.0f}, 24.0f, 1.0f, textCol);
    DrawLine(startX, startY + 45.0f, startX + panelW, startY + 45.0f, outlineCol);

    if (state.codeText.empty()) return;

    float y = startY + 55.0f;
    for (int i = 0; i < state.codeText.size(); i++) {
        if (i == state.codeLineIndex) {
            // Highlight Box
            DrawRectangle(startX, y - 2.0f, panelW, lineSpacing, config.isDarkMode ? Color{80, 85, 95, 255} : Color{220, 190, 190, 255});

            // 🌟 LOCKED TO 20px, USING BOLD FONT TO POP!
            DrawTextEx(g_App->boldFont, state.codeText[i].c_str(), {startX + 15.0f, y + 4.0f}, 20.0f, 1.0f, textCol);
        } else {
            // 🌟 LOCKED TO 20px, FADED
            DrawTextEx(g_App->mainFont, state.codeText[i].c_str(), {startX + 15.0f, y + 4.0f}, 20.0f, 1.0f, Fade(textCol, 0.6f));
        }
        y += lineSpacing;
    }
}

void GraphState::DrawSettingsModal() {
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