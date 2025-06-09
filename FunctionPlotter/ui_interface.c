#include "ui_interface.h"
#include <stdio.h>
#include <string.h>

void InitUI(UIInterface* ui) {
    memset(ui, 0, sizeof(UIInterface));

    // Initialize panels
    ui->leftPanel = (Rectangle){ 0, 0, PANEL_WIDTH, 600 };
    ui->bottomPanel = (Rectangle){ PANEL_WIDTH, 600, 1200, 100 };
    ui->statusBar = (Rectangle){ 0, 700, 1400, 50 };

    // Initialize settings
    ui->showGrid = true;
    ui->showMarkers = true;
    ui->autoScale = true;
    ui->selectedWindow = 0;

    // Initialize input fields
    strcpy(ui->xMinInput, "-10.0");
    strcpy(ui->xMaxInput, "10.0");
    strcpy(ui->yMinInput, "-5.0");
    strcpy(ui->yMaxInput, "5.0");

    strcpy(ui->statusText, "Ready - Press numbers 1-0 to toggle functions");
}

void UpdateUI(UIInterface* ui) {
    UpdateButtons(ui);
    HandleTextInput(ui);

    Vector2 mousePos = GetMousePosition();

    // Update mouse world position (this should be set by main application)
    char posText[64];
    snprintf(posText, sizeof(posText), "Mouse: (%.2f, %.2f)",
        ui->mouseWorldPos.x, ui->mouseWorldPos.y);
}

void RenderUI(UIInterface* ui) {
    DrawLeftPanel(ui);
    DrawBottomPanel(ui);
    DrawStatusBar(ui);
}

void CleanupUI(UIInterface* ui) {
    // Nothing to cleanup for now
}

int AddButton(UIInterface* ui, Rectangle bounds, const char* text, Color color) {
    if (ui->buttonCount >= MAX_UI_BUTTONS) return -1;

    int index = ui->buttonCount;
    ui->buttons[index].bounds = bounds;
    strcpy(ui->buttons[index].text, text);
    ui->buttons[index].color = color;
    ui->buttons[index].enabled = true;
    ui->buttons[index].pressed = false;

    ui->buttonCount++;
    return index;
}

bool IsButtonPressed(UIInterface* ui, int buttonIndex) {
    if (buttonIndex < 0 || buttonIndex >= ui->buttonCount) return false;
    return ui->buttons[buttonIndex].pressed;
}

void UpdateButtons(UIInterface* ui) {
    Vector2 mousePos = GetMousePosition();
    bool mousePressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

    for (int i = 0; i < ui->buttonCount; i++) {
        UIButton* button = &ui->buttons[i];

        if (CheckCollisionPointRec(mousePos, button->bounds)) {
            if (mousePressed && button->enabled) {
                button->pressed = true;
            }
        }
        else {
            button->pressed = false;
        }
    }
}

void DrawButtons(UIInterface* ui) {
    for (int i = 0; i < ui->buttonCount; i++) {
        UIButton* button = &ui->buttons[i];

        Color buttonColor = button->enabled ? button->color : LIGHTGRAY;
        if (button->pressed) {
            buttonColor = DARKGRAY;
        }

        DrawRectangleRec(button->bounds, buttonColor);
        DrawRectangleLinesEx(button->bounds, 1, BLACK);

        Vector2 textSize = MeasureTextEx(GetFontDefault(), button->text, 16, 1);
        Vector2 textPos = {
            button->bounds.x + (button->bounds.width - textSize.x) / 2,
            button->bounds.y + (button->bounds.height - textSize.y) / 2
        };

        DrawText(button->text, (int)textPos.x, (int)textPos.y, 16,
            button->enabled ? BLACK : GRAY);
    }
}

void DrawLeftPanel(UIInterface* ui) {
    // Panel background
    DrawRectangleRec(ui->leftPanel, LIGHTGRAY);
    DrawRectangleLinesEx(ui->leftPanel, 2, BLACK);

    // Title
    DrawText("Function Plotter", 10, 10, 18, BLACK);
    DrawLine(10, 35, PANEL_WIDTH - 10, 35, BLACK);

    DrawFunctionControls(ui);
    DrawWindowControls(ui);
    DrawSettingsControls(ui);
}

void DrawBottomPanel(UIInterface* ui) {
    DrawRectangleRec(ui->bottomPanel, LIGHTGRAY);
    DrawRectangleLinesEx(ui->bottomPanel, 2, BLACK);

    // Range controls
    int startX = (int)ui->bottomPanel.x + 10;
    int startY = (int)ui->bottomPanel.y + 10;

    DrawText("Axis Ranges:", startX, startY, 16, BLACK);

    DrawText("X Min:", startX, startY + 25, 14, BLACK);
    Rectangle xMinRect = { startX + 45, startY + 23, 60, 20 };
    DrawRectangleRec(xMinRect, WHITE);
    DrawRectangleLinesEx(xMinRect, 1, BLACK);
    DrawText(ui->xMinInput, (int)xMinRect.x + 2, (int)xMinRect.y + 2, 14, BLACK);

    DrawText("X Max:", startX + 120, startY + 25, 14, BLACK);
    Rectangle xMaxRect = { startX + 165, startY + 23, 60, 20 };
    DrawRectangleRec(xMaxRect, WHITE);
    DrawRectangleLinesEx(xMaxRect, 1, BLACK);
    DrawText(ui->xMaxInput, (int)xMaxRect.x + 2, (int)xMaxRect.y + 2, 14, BLACK);

    DrawText("Y Min:", startX + 240, startY + 25, 14, BLACK);
    Rectangle yMinRect = { startX + 285, startY + 23, 60, 20 };
    DrawRectangleRec(yMinRect, WHITE);
    DrawRectangleLinesEx(yMinRect, 1, BLACK);
    DrawText(ui->yMinInput, (int)yMinRect.x + 2, (int)yMinRect.y + 2, 14, BLACK);

    DrawText("Y Max:", startX + 360, startY + 25, 14, BLACK);
    Rectangle yMaxRect = { startX + 405, startY + 23, 60, 20 };
    DrawRectangleRec(yMaxRect, WHITE);
    DrawRectangleLinesEx(yMaxRect, 1, BLACK);
    DrawText(ui->yMaxInput, (int)yMaxRect.x + 2, (int)yMaxRect.y + 2, 14, BLACK);

    // Control instructions
    DrawText("Controls: Mouse Wheel (Zoom), Left Click+Drag (Pan), C (Toggle Cursors), Enter (Apply Cursor Range)",
        startX + 500, startY + 27, 12, DARKGRAY);
}

void DrawStatusBar(UIInterface* ui) {
    DrawRectangleRec(ui->statusBar, DARKGRAY);
    DrawText(ui->statusText, 10, (int)ui->statusBar.y + 15, 16, WHITE);

    // Mouse position
    char posText[128];
    snprintf(posText, sizeof(posText), "World Position: (%.3f, %.3f)",
        ui->mouseWorldPos.x, ui->mouseWorldPos.y);
    DrawText(posText, 400, (int)ui->statusBar.y + 15, 16, WHITE);
}

void DrawFunctionControls(UIInterface* ui) {
    int startY = 50;
    DrawText("Functions (1-0 keys):", 10, startY, 14, BLACK);

    const char* functionNames[] = {
        "1. sin(x)", "2. cos(x)", "3. tan(x)", "4. e^x", "5. ln(x)",
        "6. √x", "7. x", "8. x²", "9. x³", "0. |x|"
    };

    const Color functionColors[] = {
        RED, BLUE, GREEN, ORANGE, PURPLE,
        BROWN, BLACK, DARKBLUE, DARKGREEN, MAROON
    };

    for (int i = 0; i < 10; i++) {
        int y = startY + 25 + (i * 25);