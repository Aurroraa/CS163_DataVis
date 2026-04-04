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

    DrawText("Graph Data:", 10, 45, 20, textCol);

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
        int lineY = textBoxRect.y + i * 22;

        // Draw Gutter Numbers
        std::string lineNum = std::to_string(i + 1);
        int numWidth = MeasureText(lineNum.c_str(), 18);
        DrawText(lineNum.c_str(), gutterRect.x + (gutterRect.width - numWidth) / 2, lineY + 2, 18, DARKGRAY);

        // Active line highlight
        if (!selectAll && isEditorActive && cursorPos >= charCounter && cursorPos <= charCounter + lines[i].length()) {
            // 🌟 THE FIX: Adapt the highlight to Dark Mode!
            Color activeLineBg = config.isDarkMode ? Color{65, 70, 80, 255} : Color{240, 245, 255, 255};
            DrawRectangle(gutterRect.width, lineY, textBoxRect.width - gutterRect.width, 22, activeLineBg);
        }

        DrawText(lines[i].c_str(), gutterRect.width + 10, lineY + 2, 18, textCol);

        // 🌟 CALCULATE MOUSE CLICK INDEX
        if (clicked && isEditorActive && CheckCollisionPointRec(mousePos, textBoxRect)) {
            // Did we click on THIS line?
            if (mousePos.y >= lineY && mousePos.y < lineY + 22) {
                int clickLocalX = mousePos.x - (gutterRect.width + 10);
                if (clickLocalX < 0) clickLocalX = 0;

                int bestIdx = 0;
                for(int c = 0; c <= lines[i].length(); c++) {
                    int w = MeasureText(lines[i].substr(0, c).c_str(), 18);
                    if (w >= clickLocalX - 5) { bestIdx = c; break; } // -5 adds a nice hitbox bias
                    bestIdx = c;
                }
                cursorPos = charCounter + bestIdx;
                selectAll = false;
            }
        }

        // 🌟 CALCULATE CURSOR DRAW POSITION
        if (cursorPos >= charCounter && cursorPos <= charCounter + lines[i].length()) {
            std::string subStr = lines[i].substr(0, cursorPos - charCounter);
            cursorPixelX = gutterRect.width + 10 + MeasureText(subStr.c_str(), 18);
            cursorPixelY = lineY;
        }

        charCounter += lines[i].length() + 1; // +1 accounts for the \n that was removed
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

    // 🌟 CUSTOM EYE-CATCHING HOME BUTTON
    float btnX = GetScreenWidth() - 150;
    float homeBtnX = btnX - 110;
    Rectangle homeRect = {homeBtnX, 10, 90, 36};
    bool hoverHome = CheckCollisionPointRec(mousePos, homeRect);

    // Draw rounded background (Accent color if hovered)
    Color homeBg = hoverHome ? (config.isDarkMode ? Color{162, 151, 137, 255} : Color{242, 182, 182, 255}) : Fade(textCol, 0.1f);
    DrawRectangleRounded(homeRect, 0.5f, 8, homeBg);
    DrawRectangleRoundedLines(homeRect, 0.5f, 8, textCol);

    int textW = MeasureText("Home", 18);
    DrawText("Home", homeRect.x + homeRect.width/2 - textW/2, homeRect.y + 10, 18, hoverHome ? (config.isDarkMode ? BLACK : WHITE) : textCol);

    if (hoverHome && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        g_App->ChangeState(new SelectState());
    }

    GraphRenderer::Draw(Visualizer::Instance().GetRenderState(), config);
    DrawToolbar();
    DrawPseudocode();
    DrawPlayback();

    btnX = GetScreenWidth() - 150;
    Rectangle burgerRect = {btnX, 10, 40, 30};
    // Check for click manually to avoid the ugly grey button background!
    mousePos = GetMousePosition();
    bool isHoveringBurger = CheckCollisionPointRec(mousePos, burgerRect);

    if (isHoveringBurger && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        isSettingsOpen = !isSettingsOpen;
    }

    // Draw a subtle background ONLY when hovering for a nice UX feel
    if (isHoveringBurger) {
        DrawRectangleRounded(burgerRect, 0.2f, 4, config.isDarkMode ? Color{80, 85, 95, 255} : Color{220, 220, 220, 255});
    }

    // Draw the 3 smooth rounded lines perfectly centered
    DrawRectangleRounded({btnX + 8, 14, 24, 4}, 1.0f, 4, textCol);
    DrawRectangleRounded({btnX + 8, 23, 24, 4}, 1.0f, 4, textCol);
    DrawRectangleRounded({btnX + 8, 32, 24, 4}, 1.0f, 4, textCol);

    // Render Modal last so it draws on top!
    if (isSettingsOpen) DrawSettingsModal();
}

void GraphState::DrawToolbar() {
    Color panelBg = config.isDarkMode ? Color{57, 62, 70, 255} : Color{245, 232, 232, 255};
    Color outlineCol = config.isDarkMode ? Color{162, 151, 137, 255} : Color{238, 217, 217, 255};
    Color textCol = config.isDarkMode ? Color{226, 215, 193, 255} : Color{40, 40, 40, 255};

    // Accent colors for primary actions
    Color primaryBg = config.isDarkMode ? Color{162, 151, 137, 255} : Color{242, 182, 182, 255};
    Color primaryHover = ColorBrightness(primaryBg, 0.2f); // Built-in raygui color brightness

    float startY = GetScreenHeight() - 200;
    DrawRectangle(0, startY, GetScreenWidth(), 200, panelBg);
    DrawLine(0, startY, GetScreenWidth(), startY, outlineCol);

    // 🌟 HELPER: BEAUTIFUL CUSTOM ROUNDED BUTTON
    auto DrawModernBtn = [&](Rectangle rect, const char* text, bool isPrimary) -> bool {
        Vector2 mouse = GetMousePosition();
        bool isHovering = CheckCollisionPointRec(mouse, rect);

        Color bg = Fade(textCol, 0.05f); // Default subtle background
        if (isPrimary) bg = isHovering ? Fade(primaryBg, 0.8f) : primaryBg;
        else if (isHovering) bg = Fade(textCol, 0.15f);

        DrawRectangleRounded(rect, 0.4f, 8, bg);
        DrawRectangleRoundedLines(rect, 0.4f, 8, isPrimary ? bg : outlineCol);

        Color labelCol = isPrimary ? (config.isDarkMode ? BLACK : WHITE) : textCol;
        int tw = MeasureText(text, 18);
        DrawText(text, rect.x + rect.width/2 - tw/2, rect.y + rect.height/2 - 9, 18, labelCol);

        return isHovering && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    };

    float x = 30; float y = startY + 25;

    // --- 🚀 ALGORITHM GROUP (LEFT SIDE) ---
    DrawText("Algorithms", x, y, 20, textCol);
    DrawLine(x, y + 25, x + 250, y + 25, outlineCol);

    if (DrawModernBtn({x, y + 40, 250, 40}, "Run Kruskal (MST)", true)) {
        isAlgorithmActive = true;
        Visualizer::Instance().ClearHistory();
        graph.runKruskalMST();
        Visualizer::Instance().SetStep(0);
        Visualizer::Instance().SetPlaying(true);
    }

    DrawText("Src:", x + 5, y + 105, 18, textCol);
    if (GuiTextBox((Rectangle){x + 45, y + 95, 40, 40}, dijkstraSourceBuffer, 16, isDijkstraSourceActive)) {
        isDijkstraSourceActive = !isDijkstraSourceActive;
    }

    if (DrawModernBtn({x + 95, y + 95, 155, 40}, "Run Dijkstra", true)) {
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
    float rx = 320;
    DrawText("Sandbox Tools", rx, y, 20, textCol);
    DrawLine(rx, y + 25, rx + 400, y + 25, outlineCol);

    if (DrawModernBtn({rx, y + 40, 120, 40}, "Random", false)) {
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

    if (DrawModernBtn({rx + 130, y + 40, 120, 40}, "Load File", false)) {
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

    if (DrawModernBtn({rx, y + 95, 250, 40}, "Clear Screen", false)) {
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

void GraphState::DrawPseudocode() {
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

void GraphState::DrawSettingsModal() {
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
