#ifndef COORDINATE_SYSTEM_H
#define COORDINATE_SYSTEM_H

#include "raylib.h"
#include "math_functions.h"
#include <stdbool.h>

#define MAX_GRID_DIVISIONS 20
#define MIN_GRID_SPACING 20

typedef struct {
    Vector2 position;
    Vector2 size;
    Camera2D camera;

    float xMin, xMax;
    float yMin, yMax;

    float gridSpacingX, gridSpacingY;
    float scaleX, scaleY;
    bool autoScale;

    Vector2 cursorA, cursorB;
    bool cursorActive;
    bool cursorASelected, cursorBSelected;

    FunctionList activeFunctions;

    bool isActive;
    bool isDragging;
    Vector2 lastMousePos;

    bool showGrid;
    bool showAxisLabels;
    bool showMarkers;
    Color backgroundColor;

} CoordinateSystem;

void InitCoordinateSystem(CoordinateSystem* cs, Vector2 position, Vector2 size);
void UpdateCoordinateSystem(CoordinateSystem* cs);
void RenderCoordinateSystem(CoordinateSystem* cs);
void CleanupCoordinateSystem(CoordinateSystem* cs);

void UpdateAxisRanges(CoordinateSystem* cs);
void DrawGrid(CoordinateSystem* cs);
void DrawAxes(CoordinateSystem* cs);
void DrawAxisLabels(CoordinateSystem* cs);
void DrawMarkers(CoordinateSystem* cs);

Vector2 WorldToScreen(CoordinateSystem* cs, Vector2 worldPos);
Vector2 ScreenToWorld(CoordinateSystem* cs, Vector2 screenPos);

void HandleZoom(CoordinateSystem* cs, float wheelMove);
void HandlePanning(CoordinateSystem* cs);
void ResetView(CoordinateSystem* cs);
void FitToData(CoordinateSystem* cs);

void UpdateCursors(CoordinateSystem* cs);
void DrawCursors(CoordinateSystem* cs);
void ApplyCursorRange(CoordinateSystem* cs);

bool IsPointInWindow(CoordinateSystem* cs, Vector2 point);
void SetAxisRange(CoordinateSystem* cs, float xMin, float xMax, float yMin, float yMax);

#endif