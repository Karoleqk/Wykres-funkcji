#ifndef UI_INTERFACE_H
#define UI_INTERFACE_H

#include "raylib.h"
#include "math_functions.h"
#include <stdbool.h>

#define MAX_UI_BUTTONS 20
#define BUTTON_HEIGHT 30
#define PANEL_WIDTH 200

typedef struct {
    Rectangle bounds;
    char text[64];
    bool pressed;
    bool enabled;
    Color color;
} UIButton;

typedef struct {
    UIButton buttons[MAX_UI_BUTTONS];
    int buttonCount;

    Rectangle leftPanel;
    Rectangle bottomPanel;
    Rectangle statusBar;

    bool functionButtons[FUNC_COUNT];

    int selectedWindow;
    bool showWindowControls;

    bool showGrid;
    bool showMarkers;
    bool autoScale;
    
    char xMinInput[16];
    char xMaxInput[16];
    char yMinInput[16];
    char yMaxInput[16];

    Vector2 mouseWorldPos;
    char statusText[256];

} UIInterface;

void InitUI(UIInterface* ui);
void UpdateUI(UIInterface* ui);
void RenderUI(UIInterface* ui);
void CleanupUI(UIInterface* ui);

int AddButton(UIInterface* ui, Rectangle bounds, const char* text, Color color);
bool IsButtonPressed(UIInterface* ui, int buttonIndex);
void UpdateButtons(UIInterface* ui);
void DrawButtons(UIInterface* ui);

void DrawLeftPanel(UIInterface* ui);
void DrawBottomPanel(UIInterface* ui);
void DrawStatusBar(UIInterface* ui);

#endif