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
    DrawRectangle(canvasStartX, 0, GetScreenWidth() - canvasStartX, canvasHeight, RAYWHITE);
    Visualizer::Instance().DrawCanvas();

    DrawRectangle(0, 0, 260, canvasHeight, {245, 245, 245, 255});
    DrawLine(260, 0, 260, canvasHeight, DARKGRAY);

    bool prevDirected = isDirected;
    GuiCheckBox((Rectangle){10, 10, 20, 20}, "Directed Graph", &isDirected);
    if (isDirected != prevDirected) {
        Visualizer::Instance().ClearHistory();
        graph.initFromLiveText(std::string(textEditorBuffer), isDirected, canvasStartX, 0, GetScreenWidth()-canvasStartX, canvasHeight);
    }

    DrawText("Graph Data:", 10, 45, 20, BLACK);

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
    DrawRectangleRec(textBoxRect, selectAll ? Color{220, 235, 255, 255} : WHITE); // Light blue if Ctrl+A
    Rectangle gutterRect = {textBoxRect.x, textBoxRect.y, 40, textBoxRect.height};
    DrawRectangleRec(gutterRect, {230, 230, 230, 255});

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
            DrawRectangle(gutterRect.width, lineY, textBoxRect.width - gutterRect.width, 22, {240, 245, 255, 255});
        }

        DrawText(lines[i].c_str(), gutterRect.width + 10, lineY + 2, 18, BLACK);

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
        DrawLine(cursorPixelX, cursorPixelY + 2, cursorPixelX, cursorPixelY + 20, BLACK);
    }

    // ... [KEEP YOUR EXISTING HOME BUTTON & RENDERER CALLS EXACTLY THE SAME] ...
    if (GuiButton((Rectangle){(float)GetScreenWidth() - 100, 10, 80, 30}, "Home")) {
        g_App->ChangeState(new SelectState());
    }

    GraphRenderer::Draw(Visualizer::Instance().GetRenderState());
    DrawToolbar();
}

void GraphState::DrawToolbar() {
    float startY = GetScreenHeight() - 200;
    DrawRectangle(0, startY, GetScreenWidth(), 200, LIGHTGRAY);
    DrawLine(0, startY, GetScreenWidth(), startY, DARKGRAY);

    float x = 20; float y = startY + 20;

    // --- ALGORITHM BUTTONS ---
    if (GuiButton((Rectangle){x, y, 200, 40}, "Run Minimum Spanning Tree")) {
        isAlgorithmActive = true;
        Visualizer::Instance().ClearHistory();
        graph.runKruskalMST();
        Visualizer::Instance().SetStep(0);
        Visualizer::Instance().SetPlaying(true);
    }

    DrawText("Source:", x, y + 80, 18, BLACK);
    if (GuiTextBox((Rectangle){x + 80, y + 70, 40, 40}, dijkstraSourceBuffer, 16, isDijkstraSourceActive)) {
        isDijkstraSourceActive = !isDijkstraSourceActive;
    }

    if (GuiButton((Rectangle){x + 130, y + 70, 120, 40}, "Run Dijkstra")) {
        int startNode = -1;

        // Use the text box if they typed something, otherwise fallback to the first node
        if (dijkstraSourceBuffer[0] != '\0') {
            try { startNode = std::stoi(dijkstraSourceBuffer); } catch (...) {}
        } else {
            startNode = graph.getFirstVertexId();
        }

        if (startNode != -1) {
            isAlgorithmActive = true;
            Visualizer::Instance().ClearHistory();
            graph.runDijkstra(startNode);
            Visualizer::Instance().SetStep(0);
            Visualizer::Instance().SetPlaying(true);
        }
    }

    // --- UTILITY BUTTONS ---
    if (GuiButton((Rectangle){x + 230, y, 120, 40}, "Clear Screen")) {
        isAlgorithmActive = false;
        Visualizer::Instance().ClearHistory();
        graph.clear();
        textEditorBuffer[0] = '\0';
    }

    // 🌟 NEW: LOAD FILE BUTTON
    if (GuiButton((Rectangle){x + 530, y, 120, 40}, "Load File...")) {
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
    if (GuiButton((Rectangle){x + 380, y, 120, 40}, "Random")) {
        std::string randomGraph = "";
        int numNodes = GetRandomValue(4, 7); // 4 to 7 nodes so it isn't too cluttered
        int numEdges = GetRandomValue(numNodes, numNodes * 2);

        for (int i = 0; i < numEdges; i++) {
            int u = GetRandomValue(1, numNodes);
            int v = GetRandomValue(1, numNodes);

            // Prevent self-loops
            while (u == v) {
                v = GetRandomValue(1, numNodes);
            }

            int w = GetRandomValue(1, 99); // Random weight

            randomGraph += std::to_string(u) + " " + std::to_string(v) + " " + std::to_string(w) + "\n";
        }

        // Dump the generated text into the UI buffer!
        strncpy(textEditorBuffer, randomGraph.c_str(), 2047);
        textEditorBuffer[2047] = '\0';

    }
}

void GraphState::DrawPlayback() {
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

void GraphState::DrawPseudocode() {
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

