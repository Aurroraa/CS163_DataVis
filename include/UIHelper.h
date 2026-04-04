#pragma once
#include "raylib.h"
#include "Global.h"

namespace UIHelper {
    // --- GLOBAL PALETTES ---
    inline Color GetCanvasBg(const UIConfig& config) { return config.isDarkMode ? Color{35, 41, 49, 255} : Color{250, 250, 250, 255}; }
    inline Color GetPanelBg(const UIConfig& config) { return config.isDarkMode ? Color{57, 62, 70, 255} : Color{245, 232, 232, 255}; }
    inline Color GetTextCol(const UIConfig& config) { return config.isDarkMode ? Color{226, 215, 193, 255} : Color{40, 40, 40, 255}; }
    inline Color GetOutlineCol(const UIConfig& config) { return config.isDarkMode ? Color{162, 151, 137, 255} : Color{238, 217, 217, 255}; }
    inline Color GetPrimaryCol(const UIConfig& config) { return config.isDarkMode ? Color{162, 151, 137, 255} : Color{242, 182, 182, 255}; }

    // --- GLOBAL MODERN BUTTON ---
    inline bool DrawModernBtn(Rectangle rect, const char* text, bool isPrimary, const UIConfig& config) {
        Vector2 mouse = GetMousePosition();
        bool isHovering = CheckCollisionPointRec(mouse, rect);
        Color textCol = GetTextCol(config);
        Color primaryBg = GetPrimaryCol(config);
        
        Color bg = Fade(textCol, 0.05f); 
        if (isPrimary) bg = isHovering ? Fade(primaryBg, 0.8f) : primaryBg;
        else if (isHovering) bg = Fade(textCol, 0.15f);

        DrawRectangleRounded(rect, 0.4f, 8, bg);
        DrawRectangleRoundedLines(rect, 0.4f, 8, isPrimary ? bg : GetOutlineCol(config));
        
        Color labelCol = isPrimary ? (config.isDarkMode ? BLACK : WHITE) : textCol;
        int tw = MeasureText(text, 18);
        DrawText(text, rect.x + rect.width/2 - tw/2, rect.y + rect.height/2 - 9, 18, labelCol);
        
        return isHovering && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    }
    // --- GLOBAL MODERN SLIDER ---
    inline void DrawModernSlider(Rectangle rect, const char* leftText, const char* rightText, float* value, float minValue, float maxValue, const UIConfig& config) {
        Vector2 mouse = GetMousePosition();

        // Expanded hitbox so you don't have to click perfectly on the thin line
        Rectangle hitBox = {rect.x - 10, rect.y - 10, rect.width + 20, rect.height + 20};
        bool isHovering = CheckCollisionPointRec(mouse, hitBox);

        // Logic: Map mouse X position to the value
        if (isHovering && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            float percent = (mouse.x - rect.x) / rect.width;
            if (percent < 0.0f) percent = 0.0f;
            if (percent > 1.0f) percent = 1.0f;
            *value = minValue + percent * (maxValue - minValue);
        }

        Color textCol = GetTextCol(config);
        Color trackCol = Fade(GetOutlineCol(config), 0.5f);
        Color primaryCol = GetPrimaryCol(config);

        float percent = (*value - minValue) / (maxValue - minValue);
        float knobX = rect.x + percent * rect.width;
        float centerY = rect.y + rect.height / 2.0f;

        // Draw Background Track
        DrawRectangleRounded({rect.x, centerY - 3, rect.width, 6}, 1.0f, 4, trackCol);

        // Draw Active Filled Track (From start to knob)
        DrawRectangleRounded({rect.x, centerY - 3, percent * rect.width, 6}, 1.0f, 4, primaryCol);

        // Draw Knob
        Color knobCol = isHovering ? (config.isDarkMode ? WHITE : DARKGRAY) : primaryCol;
        DrawCircle(knobX, centerY, 8.0f, knobCol);
        DrawCircleLines(knobX, centerY, 8.0f, config.isDarkMode ? BLACK : WHITE); // Small outline for pop

        // Draw Text Labels
        int leftW = MeasureText(leftText, 16);
        DrawText(leftText, rect.x - leftW - 10, centerY - 8, 16, textCol);
        DrawText(rightText, rect.x + rect.width + 10, centerY - 8, 16, textCol);
    }
    // --- GLOBAL HAMBURGER BUTTON ---
    inline bool DrawHamburgerBtn(Rectangle rect, const UIConfig& config) {
        float btnX = GetScreenWidth() - 150;
        Rectangle burgerRect = {btnX, 10, 40, 30};
        // Check for click manually to avoid the ugly grey button background!
        Vector2 mousePos = GetMousePosition();
        bool isHoveringBurger = CheckCollisionPointRec(mousePos, burgerRect);

        // Draw a subtle background ONLY when hovering for a nice UX feel
        if (isHoveringBurger) {
            DrawRectangleRounded(burgerRect, 0.2f, 4, config.isDarkMode ? Color{80, 85, 95, 255} : Color{220, 220, 220, 255});
        }

        // Draw the 3 smooth rounded lines perfectly centered
        Color textCol = GetTextCol(config);
        DrawRectangleRounded({btnX + 8, 14, 24, 4}, 1.0f, 4, textCol);
        DrawRectangleRounded({btnX + 8, 23, 24, 4}, 1.0f, 4, textCol);
        DrawRectangleRounded({btnX + 8, 32, 24, 4}, 1.0f, 4, textCol);

        return isHoveringBurger && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    }
}