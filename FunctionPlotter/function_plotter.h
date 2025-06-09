#ifndef FUNCTION_PLOTTER_H
#define FUNCTION_PLOTTER_H

#include "raylib.h"
#include "math_functions.h"
#include "coordinate_system.h"
#include "ui_interface.h"
#include <stdbool.h>

#define MAX_WINDOWS 4
#define SCREEN_WIDTH 1400
#define SCREEN_HEIGHT 900

typedef struct {
    CoordinateSystem windows[MAX_WINDOWS];
    UIInterface ui;
    int activeWindows;
    int selectedWindow;
    bool isInitialized;
} PlotterState;

bool InitializePlotter(void);
void RunPlotter(void);
void ShutdownPlotter(void);

void HandleInput(void);
void HandleWindowEvents(void);
void HandleFunctionToggle(void);

void RenderApplication(void);

#endif