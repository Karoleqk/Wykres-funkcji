#include "coordinate_system.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static float ClampFloat(float value, float min, float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

void InitCoordinateSystem(CoordinateSystem* cs, Vector2 position, Vector2 size) {
    memset(cs, 0, sizeof(CoordinateSystem));

    cs->position = position;
    cs->size = size;
    cs->isActive = true;

    // Initialize camera
    cs->camera.zoom = 1.0f;
    cs->camera.target = (Vector2){ 0, 0 };
    cs->camera.offset = (Vector2){ size.x / 2, size.y / 2 };
    cs->camera.rotation = 0.0f;

    // Default axis ranges
    cs->xMin = -10.0f;
    cs->xMax = 10.0f;
    cs->yMin = -5.0f;
    cs->yMax = 5.0f;

    // Grid settings
    cs->gridSpacingX = 1.0f;
    cs->gridSpacingY = 1.0f;
    cs->scaleX = size.x / (cs->xMax - cs->xMin);
    cs->scaleY = size.y / (cs->yMax - cs->yMin);
    cs->autoScale = true;

    // Visual settings
    cs->showGrid = true;
    cs->showAxisLabels = true;
    cs->showMarkers = true;
    cs->backgroundColor = RAYWHITE;

    // Initialize cursors
    cs->cursorA = (Vector2){ cs->xMin + 2, 0 };
    cs->cursorB = (Vector2){ cs->xMax - 2, 0 };
    cs->cursorActive = false;

    // Initialize function list
    InitFunctionList(&cs->activeFunctions);
}

void UpdateCoordinateSystem(CoordinateSystem* cs) {
    if (!cs->isActive) return;

    Vector2 mousePos = GetMousePosition();
    bool mouseInWindow = IsPointInWindow(cs, mousePos);

    if (mouseInWindow) {
        // Handle zooming
        float wheelMove = GetMouseWheelMove();
        if (wheelMove != 0) {
            HandleZoom(cs, wheelMove);
        }

        // Handle panning
        HandlePanning(cs);

        // Update cursors
        UpdateCursors(cs);
    }

    // Update axis ranges if auto-scaling
    if (cs->autoScale) {
        UpdateAxisRanges(cs);
    }

    // Update scale factors
    cs->scaleX = cs->size.x / (cs->xMax - cs->xMin);
    cs->scaleY = cs->size.y / (cs->yMax - cs->yMin);
}

void RenderCoordinateSystem(CoordinateSystem* cs) {
    if (!cs->isActive) return;

    // Set scissor rectangle for this window
    BeginScissorMode((int)cs->position.x, (int)cs->position.y,
        (int)cs->size.x, (int)cs->size.y);

    // Clear background
    DrawRectangle((int)cs->position.x, (int)cs->position.y,
        (int)cs->size.x, (int)cs->size.y, cs->backgroundColor);

    // Begin 2D mode with camera
    BeginMode2D(cs->camera);

    // Draw grid
    if (cs->showGrid) {
        DrawGrid(cs);
    }

    // Draw axes
    DrawAxes(cs);

    // Draw functions
    FunctionNode* node = cs->activeFunctions.first;
    while (node != NULL) {
        if (node->function.visible) {
            DrawFunctionInRange(&node->function, cs->xMin, cs->xMax,
                cs->yMin, cs->yMax,
                (Rectangle) {
                cs->position.x, cs->position.y, cs->size.x, cs->size.y
            });
        }
        node = node->next;
    }

    // Draw cursors
    if (cs->cursorActive) {
        DrawCursors(cs);
    }

    EndMode2D();

    // Draw axis labels and markers (outside 2D mode)
    if (cs->showAxisLabels) {
        DrawAxisLabels(cs);
    }

    if (cs->showMarkers) {
        DrawMarkers(cs);
    }

    EndScissorMode();

    // Draw window border
    DrawRectangleLinesEx((Rectangle) { cs->position.x, cs->position.y, cs->size.x, cs->size.y },
        1, LIGHTGRAY);
}

void CleanupCoordinateSystem(CoordinateSystem* cs) {
    ClearFunctionList(&cs->activeFunctions);
}

void UpdateAxisRanges(CoordinateSystem* cs) {
    if (cs->activeFunctions.count == 0) return;

    float globalYMin = INFINITY, globalYMax = -INFINITY;
    bool hasValidBounds = false;

    FunctionNode* node = cs->activeFunctions.first;
    while (node != NULL) {
        if (node->function.visible) {
            float yMin, yMax;
            GetFunctionBounds(&node->function, cs->xMin, cs->xMax, &yMin, &yMax);

            if (IsValidFunctionValue(yMin) && IsValidFunctionValue(yMax)) {
                if (yMin < globalYMin) globalYMin = yMin;
                if (yMax > globalYMax) globalYMax = yMax;
                hasValidBounds = true;
            }
        }
        node = node->next;
    }

    if (hasValidBounds) {
        float margin = (globalYMax - globalYMin) * 0.1f;
        cs->yMin = globalYMin - margin;
        cs->yMax = globalYMax + margin;
    }
}

void DrawGrid(CoordinateSystem* cs) {
    // Calculate grid spacing based on zoom level
    float spacing = cs->gridSpacingX / cs->camera.zoom;
    if (spacing < 0.1f) spacing = 0.1f;
    if (spacing > 10.0f) spacing = 10.0f;

    // Vertical grid lines
    for (float x = cs->xMin; x <= cs->xMax; x += spacing) {
        Vector2 start = WorldToScreen(cs, (Vector2) { x, cs->yMin });
        Vector2 end = WorldToScreen(cs, (Vector2) { x, cs->yMax });
        DrawLineEx(start, end, 1.0f, LIGHTGRAY);
    }

    // Horizontal grid lines
    spacing = cs->gridSpacingY / cs->camera.zoom;
    if (spacing < 0.1f) spacing = 0.1f;
    if (spacing > 10.0f) spacing = 10.0f;

    for (float y = cs->yMin; y <= cs->yMax; y += spacing) {
        Vector2 start = WorldToScreen(cs, (Vector2) { cs->xMin, y });
        Vector2 end = WorldToScreen(cs, (Vector2) { cs->xMax, y });
        DrawLineEx(start, end, 1.0f, LIGHTGRAY);
    }
}

void DrawAxes(CoordinateSystem* cs) {
    // X-axis
    if (cs->yMin <= 0 && cs->yMax >= 0) {
        Vector2 start = WorldToScreen(cs, (Vector2) { cs->xMin, 0 });
        Vector2 end = WorldToScreen(cs, (Vector2) { cs->xMax, 0 });
        DrawLineEx(start, end, 2.0f, BLACK);
    }

    // Y-axis
    if (cs->xMin <= 0 && cs->xMax >= 0) {
        Vector2 start = WorldToScreen(cs, (Vector2) { 0, cs->yMin });
        Vector2 end = WorldToScreen(cs, (Vector2) { 0, cs->yMax });
        DrawLineEx(start, end, 2.0f, BLACK);
    }
}

void DrawAxisLabels(CoordinateSystem* cs) {
    // X-axis labels
    float spacing = 1.0f;
    for (float x = cs->xMin; x <= cs->xMax; x += spacing) {
        Vector2 pos = WorldToScreen(cs, (Vector2) { x, 0 });
        pos.x += cs->position.x;
        pos.y += cs->position.y + 15;

        if (pos.x >= cs->position.x && pos.x <= cs->position.x + cs->size.x) {
            DrawText(TextFormat("%.1f", x), (int)pos.x - 10, (int)pos.y, 12, DARKGRAY);
        }
    }

    // Y-axis labels
    for (float y = cs->yMin; y <= cs->yMax; y += spacing) {
        Vector2 pos = WorldToScreen(cs, (Vector2) { 0, y });
        pos.x += cs->position.x - 25;
        pos.y += cs->position.y;

        if (pos.y >= cs->position.y && pos.y <= cs->position.y + cs->size.y) {
            DrawText(TextFormat("%.1f", y), (int)pos.x, (int)pos.y - 6, 12, DARKGRAY);
        }
    }
}

void DrawMarkers(CoordinateSystem* cs) {
    // X-axis markers
    float spacing = 1.0f;
    for (float x = cs->xMin; x <= cs->xMax; x += spacing) {
        Vector2 pos = WorldToScreen(cs, (Vector2) { x, 0 });
        pos.x += cs->position.x;
        pos.y += cs->position.y;

        DrawLineEx((Vector2) { pos.x, pos.y - 5 }, (Vector2) { pos.x, pos.y + 5 }, 2.0f, BLACK);
    }

    // Y-axis markers
    for (float y = cs->yMin; y <= cs->yMax; y += spacing) {
        Vector2 pos = WorldToScreen(cs, (Vector2) { 0, y });
        pos.x += cs->position.x;
        pos.y += cs->position.y;

        DrawLineEx((Vector2) { pos.x - 5, pos.y }, (Vector2) { pos.x + 5, pos.y }, 2.0f, BLACK);
    }
}

Vector2 WorldToScreen(CoordinateSystem* cs, Vector2 worldPos) {
    return GetWorldToScreen2D(worldPos, cs->camera);
}

Vector2 ScreenToWorld(CoordinateSystem* cs, Vector2 screenPos) {
    Vector2 localPos = { screenPos.x - cs->position.x, screenPos.y - cs->position.y };
    return GetScreenToWorld2D(localPos, cs->camera);
}

void HandleZoom(CoordinateSystem* cs, float wheelMove) {
    Vector2 mousePos = GetMousePosition();
    Vector2 localMousePos = { mousePos.x - cs->position.x, mousePos.y - cs->position.y };
    Vector2 worldMousePos = GetScreenToWorld2D(localMousePos, cs->camera);

    cs->camera.offset = localMousePos;
    cs->camera.target = worldMousePos;

    float scaleFactor = 1.0f + (wheelMove * 0.1f);
    cs->camera.zoom = ClampFloat(cs->camera.zoom * scaleFactor, 0.1f, 10.0f);

    // Update axis ranges based on new zoom
    float halfWidth = cs->size.x / (2.0f * cs->camera.zoom * cs->scaleX);
    float halfHeight = cs->size.y / (2.0f * cs->camera.zoom * cs->scaleY);

    cs->xMin = cs->camera.target.x - halfWidth;
    cs->xMax = cs->camera.target.x + halfWidth;
    cs->yMin = cs->camera.target.y - halfHeight;
    cs->yMax = cs->camera.target.y + halfHeight;
}

void HandlePanning(CoordinateSystem* cs) {
    Vector2 mousePos = GetMousePosition();
    Vector2 localMousePos = { mousePos.x - cs->position.x, mousePos.y - cs->position.y };

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        cs->lastMousePos = localMousePos;
        cs->isDragging = true;
    }

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        cs->isDragging = false;
    }

    if (cs->isDragging) {
        Vector2 delta = {
            (cs->lastMousePos.x - localMousePos.x) / cs->camera.zoom,
            (cs->lastMousePos.y - localMousePos.y) / cs->camera.zoom
        };

        cs->camera.target.x += delta.x;
        cs->camera.target.y -= delta.y; // Invert Y for correct direction

        cs->lastMousePos = localMousePos;

        // Update axis ranges
        float halfWidth = cs->size.x / (2.0f * cs->camera.zoom * cs->scaleX);
        float halfHeight = cs->size.y / (2.0f * cs->camera.zoom * cs->scaleY);

        cs->xMin = cs->camera.target.x - halfWidth;
        cs->xMax = cs->camera.target.x + halfWidth;
        cs->yMin = cs->camera.target.y - halfHeight;
        cs->yMax = cs->camera.target.y + halfHeight;
    }
}

void ResetView(CoordinateSystem* cs) {
    cs->camera.zoom = 1.0f;
    cs->camera.target = (Vector2){ 0, 0 };
    cs->camera.offset = (Vector2){ cs->size.x / 2, cs->size.y / 2 };

    cs->xMin = -10.0f;
    cs->xMax = 10.0f;
    cs->yMin = -5.0f;
    cs->yMax = 5.0f;
}

void FitToData(CoordinateSystem* cs) {
    if (cs->activeFunctions.count == 0) return;

    float globalYMin = INFINITY, globalYMax = -INFINITY;
    bool hasValidBounds = false;

    FunctionNode* node = cs->activeFunctions.first;
    while (node != NULL) {
        if (node->function.visible) {
            float yMin, yMax;
            GetFunctionBounds(&node->function, cs->xMin, cs->xMax, &yMin, &yMax);

            if (IsValidFunctionValue(yMin) && IsValidFunctionValue(yMax)) {
                if (yMin < globalYMin) globalYMin = yMin;
                if (yMax > globalYMax) globalYMax = yMax;
                hasValidBounds = true;
            }
        }
        node = node->next;
    }

    if (hasValidBounds) {
        float margin = (globalYMax - globalYMin) * 0.1f;
        cs->yMin = globalYMin - margin;
        cs->yMax = globalYMax + margin;

        // Update camera to fit data
        cs->camera.target = (Vector2){ (cs->xMax + cs->xMin) / 2, (cs->yMax + cs->yMin) / 2 };

        float zoomX = cs->size.x / (cs->xMax - cs->xMin);
        float zoomY = cs->size.y / (cs->yMax - cs->yMin);
        cs->camera.zoom = fminf(zoomX, zoomY) * 0.8f; // 80% to add some margin
    }
}

void UpdateCursors(CoordinateSystem* cs) {
    Vector2 mousePos = GetMousePosition();
    Vector2 localMousePos = { mousePos.x - cs->position.x, mousePos.y - cs->position.y };
    Vector2 worldMousePos = GetScreenToWorld2D(localMousePos, cs->camera);

    // Toggle cursor mode with 'C' key
    if (IsKeyPressed(KEY_C)) {
        cs->cursorActive = !cs->cursorActive;
    }

    if (!cs->cursorActive) return;

    // Handle cursor selection and movement
    float cursorAScreenX = GetWorldToScreen2D(cs->cursorA, cs->camera).x;
    float cursorBScreenX = GetWorldToScreen2D(cs->cursorB, cs->camera).x;

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        float distToA = fabsf(localMousePos.x - cursorAScreenX);
        float distToB = fabsf(localMousePos.x - cursorBScreenX);

        if (distToA < 10) {
            cs->cursorASelected = true;
            cs->cursorBSelected = false;
        }
        else if (distToB < 10) {
            cs->cursorBSelected = true;
            cs->cursorASelected = false;
        }
    }

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        cs->cursorASelected = false;
        cs->cursorBSelected = false;
    }

    // Move selected cursor
    if (cs->cursorASelected) {
        cs->cursorA.x = worldMousePos.x;
    }
    else if (cs->cursorBSelected) {
        cs->cursorB.x = worldMousePos.x;
    }

    // Apply cursor range with Enter
    if (IsKeyPressed(KEY_ENTER) && cs->cursorActive) {
        ApplyCursorRange(cs);
    }
}

void DrawCursors(CoordinateSystem* cs) {
    // Cursor A (left)
    Vector2 cursorAScreen = GetWorldToScreen2D(cs->cursorA, cs->camera);
    cursorAScreen.x += cs->position.x;
    cursorAScreen.y = cs->position.y;

    DrawLineEx((Vector2) { cursorAScreen.x, cursorAScreen.y },
        (Vector2) {
        cursorAScreen.x, cursorAScreen.y + cs->size.y
    },
        2.0f, cs->cursorASelected ? RED : DARKGREEN);

    DrawText(TextFormat("A: %.2f", cs->cursorA.x),
        (int)cursorAScreen.x + 5, (int)cursorAScreen.y + 10, 14, DARKGREEN);

    // Cursor B (right)
    Vector2 cursorBScreen = GetWorldToScreen2D(cs->cursorB, cs->camera);
    cursorBScreen.x += cs->position.x;
    cursorBScreen.y = cs->position.y;

    DrawLineEx((Vector2) { cursorBScreen.x, cursorBScreen.y },
        (Vector2) {
        cursorBScreen.x, cursorBScreen.y + cs->size.y
    },
        2.0f, cs->cursorBSelected ? RED : DARKBLUE);

    DrawText(TextFormat("B: %.2f", cs->cursorB.x),
        (int)cursorBScreen.x + 5, (int)cursorBScreen.y + 30, 14, DARKBLUE);
}

void ApplyCursorRange(CoordinateSystem* cs) {
    float leftX = fminf(cs->cursorA.x, cs->cursorB.x);
    float rightX = fmaxf(cs->cursorA.x, cs->cursorB.x);

    SetAxisRange(cs, leftX, rightX, cs->yMin, cs->yMax);

    // Update camera to show selected range
    cs->camera.target.x = (leftX + rightX) / 2;
    float zoomX = cs->size.x / (rightX - leftX);
    cs->camera.zoom = zoomX * 0.8f;
}

bool IsPointInWindow(CoordinateSystem* cs, Vector2 point) {
    return (point.x >= cs->position.x && point.x <= cs->position.x + cs->size.x &&
        point.y >= cs->position.y && point.y <= cs->position.y + cs->size.y);
}

void SetAxisRange(CoordinateSystem* cs, float xMin, float xMax, float yMin, float yMax) {
    cs->xMin = xMin;
    cs->xMax = xMax;
    cs->yMin = yMin;
    cs->yMax = yMax;

    cs->scaleX = cs->size.x / (cs->xMax - cs->xMin);
    cs->scaleY = cs->size.y / (cs->yMax - cs->yMin);
}