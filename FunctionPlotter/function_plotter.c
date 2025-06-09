#include "function_plotter.h"
#include <stdio.h>
#include <stdlib.h>

static PlotterState g_plotter = { 0 };

bool InitializePlotter(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Advanced Function Plotter");
    SetTargetFPS(60);

    if (!IsWindowReady()) {
        return false;
    }

    // Initialize UI
    InitUI(&g_plotter.ui);

    // Initialize coordinate system windows
    g_plotter.activeWindows = 1;
    g_plotter.selectedWindow = 0;

    // Main window (full size initially)
    Vector2 mainPos = { PANEL_WIDTH, 0 };
    Vector2 mainSize = { SCREEN_WIDTH - PANEL_WIDTH, SCREEN_HEIGHT - 100 };
    InitCoordinateSystem(&g_plotter.windows[0], mainPos, mainSize);

    // Additional windows (initially hidden)
    for (int i = 1; i < MAX_WINDOWS; i++) {
        InitCoordinateSystem(&g_plotter.windows[i], (Vector2) { 0, 0 }, (Vector2) { 0, 0 });
        g_plotter.windows[i].isActive = false;
    }

    g_plotter.isInitialized = true;
    return true;
}

void RunPlotter(void) {
    while (!WindowShouldClose()) {
        HandleInput();

        // Update active coordinate systems
        for (int i = 0; i < g_plotter.activeWindows; i++) {
            if (g_plotter.windows[i].isActive) {
                UpdateCoordinateSystem(&g_plotter.windows[i]);
            }
        }

        UpdateUI(&g_plotter.ui);
        RenderApplication();
    }
}

void ShutdownPlotter(void) {
    if (!g_plotter.isInitialized) return;

    // Cleanup coordinate systems
    for (int i = 0; i < MAX_WINDOWS; i++) {
        CleanupCoordinateSystem(&g_plotter.windows[i]);
    }

    CleanupUI(&g_plotter.ui);
    CloseWindow();

    g_plotter.isInitialized = false;
}

void HandleInput(void) {
    HandleFunctionToggle();
    HandleWindowEvents();

    // Handle window selection
    if (IsKeyPressed(KEY_TAB)) {
        g_plotter.selectedWindow = (g_plotter.selectedWindow + 1) % g_plotter.activeWindows;
    }

    // Add new window
    if (IsKeyPressed(KEY_N) && IsKeyDown(KEY_LEFT_CONTROL)) {
        if (g_plotter.activeWindows < MAX_WINDOWS) {
            int newIdx = g_plotter.activeWindows;

            // Calculate position for new window
            Vector2 pos, size;
            if (g_plotter.activeWindows == 1) {
                // Split into 2x1
                size = (Vector2){ (SCREEN_WIDTH - PANEL_WIDTH) / 2, SCREEN_HEIGHT - 100 };
                g_plotter.windows[0].size = size;
                pos = (Vector2){ PANEL_WIDTH + size.x, 0 };
            }
            else if (g_plotter.activeWindows == 2) {
                // Split into 2x2
                size = (Vector2){ (SCREEN_WIDTH - PANEL_WIDTH) / 2, (SCREEN_HEIGHT - 100) / 2 };
                for (int i = 0; i < 2; i++) {
                    g_plotter.windows[i].size = size;
                }
                pos = (Vector2){ PANEL_WIDTH, size.y };
            }
            else {
                // 2x2 grid, bottom right
                size = (Vector2){ (SCREEN_WIDTH - PANEL_WIDTH) / 2, (SCREEN_HEIGHT - 100) / 2 };
                pos = (Vector2){ PANEL_WIDTH + size.x, size.y };
            }

            InitCoordinateSystem(&g_plotter.windows[newIdx], pos, size);
            g_plotter.activeWindows++;
        }
    }

    // Reset view for current window
    if (IsKeyPressed(KEY_R)) {
        ResetView(&g_plotter.windows[g_plotter.selectedWindow]);
    }

    // Fit to data for current window
    if (IsKeyPressed(KEY_F)) {
        FitToData(&g_plotter.windows[g_plotter.selectedWindow]);
    }
}

void HandleFunctionToggle(void) {
    static const int functionKeys[] = {
        KEY_ONE, KEY_TWO, KEY_THREE, KEY_FOUR,
        KEY_FIVE, KEY_SIX, KEY_SEVEN, KEY_EIGHT,
        KEY_NINE, KEY_ZERO
    };

    static const FunctionType functionTypes[] = {
        FUNC_SIN, FUNC_COS, FUNC_TAN, FUNC_EXP,
        FUNC_LOG, FUNC_SQRT, FUNC_X, FUNC_X2,
        FUNC_X3, FUNC_ABS
    };

    for (int i = 0; i < 10; i++) {
        if (IsKeyPressed(functionKeys[i])) {
            CoordinateSystem* currentWindow = &g_plotter.windows[g_plotter.selectedWindow];
            ToggleFunction(&currentWindow->activeFunctions, functionTypes[i]);
            break;
        }
    }
}

void HandleWindowEvents(void) {
    Vector2 mousePos = GetMousePosition();

    // Determine which window is being interacted with
    for (int i = 0; i < g_plotter.activeWindows; i++) {
        if (IsPointInWindow(&g_plotter.windows[i], mousePos)) {
            g_plotter.selectedWindow = i;
            break;
        }
    }
}

void RenderApplication(void) {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    // Render all active coordinate systems
    for (int i = 0; i < g_plotter.activeWindows; i++) {
        if (g_plotter.windows[i].isActive) {
            RenderCoordinateSystem(&g_plotter.windows[i]);

            // Highlight selected window
            if (i == g_plotter.selectedWindow) {
                Rectangle windowBounds = {
                    g_plotter.windows[i].position.x,
                    g_plotter.windows[i].position.y,
                    g_plotter.windows[i].size.x,
                    g_plotter.windows[i].size.y
                };
                DrawRectangleLinesEx(windowBounds, 3, BLUE);
            }
        }
    }

    // Render UI
    RenderUI(&g_plotter.ui);

    // Status information
    DrawText(TextFormat("Window: %d/%d", g_plotter.selectedWindow + 1, g_plotter.activeWindows),
        10, SCREEN_HEIGHT - 30, 20, BLACK);

    DrawText("Controls: 1-0 (Functions), Tab (Switch Window), Ctrl+N (New Window), R (Reset), F (Fit)",
        10, SCREEN_HEIGHT - 55, 14, DARKGRAY);

    EndDrawing();
}