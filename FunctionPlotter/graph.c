#include "graph.h"
#include "utils.h"
#include <stdlib.h>
#include <math.h>

Graph CreateGraph(Rectangle bounds) {
    Graph g = {
        .bounds = bounds,
        .xMin = -10, .xMax = 10,
        .yMin = -1, .yMax = 1,
        .scaleX = 1, .scaleY = 1,
        .functionCount = 0,
        .functions = NULL,
        .dragging = false,
        .xLabel = "X",
        .yLabel = "Y",
        .title = "Wykres funkcji"
    };
    return g;
}

void AddFunctionToGraph(Graph* graph, FunctionPtr func, Color color) {
    graph->functions = realloc(graph->functions, sizeof(Function) * (graph->functionCount + 1));
    graph->functions[graph->functionCount++] = (Function){ func, color };
}

void UpdateGraph(Graph* graph) {
    if (CheckCollisionPointRec(GetMousePosition(), graph->bounds)) {
        float wheel = GetMouseWheelMove();
        if (wheel != 0) {
            float factor = (wheel > 0) ? 0.9f : 1.1f;
            float mx = GetMouseX();
            float my = GetMouseY();

            float worldX = ScreenToWorldX(graph, mx);
            float worldY = ScreenToWorldY(graph, my);

            graph->xMin = worldX + (graph->xMin - worldX) * factor;
            graph->xMax = worldX + (graph->xMax - worldX) * factor;
            graph->yMin = worldY + (graph->yMin - worldY) * factor;
            graph->yMax = worldY + (graph->yMax - worldY) * factor;
        }

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            graph->dragging = true;
            graph->dragStart = GetMousePosition();
        }

        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && graph->dragging) {
            Vector2 delta = Vector2Subtract(GetMousePosition(), graph->dragStart);
            float dx = (delta.x / graph->bounds.width) * (graph->xMax - graph->xMin);
            float dy = (delta.y / graph->bounds.height) * (graph->yMax - graph->yMin);
            graph->xMin -= dx;
            graph->xMax -= dx;
            graph->yMin += dy;
            graph->yMax += dy;
            graph->dragStart = GetMousePosition();
        }

        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            graph->dragging = false;
        }
    }
}

float GetOptimalStep(float range) {
    float step = pow(10, floor(log10(range)));
    if (range / step > 5) step *= 2;
    else if (range / step < 2) step /= 2;
    return step;
}

void DrawGraph(Graph* graph) {
    DrawRectangleLinesEx(graph->bounds, 2, GRAY);

    if (graph->title) {
        int titleWidth = MeasureText(graph->title, 16);
        DrawText(graph->title,
            graph->bounds.x + (graph->bounds.width - titleWidth) / 2,
            graph->bounds.y - 25, 16, BLACK);
    }

    float zeroX = WorldToScreenX(graph, 0);
    float zeroY = WorldToScreenY(graph, 0);

    if (graph->xMin <= 0 && graph->xMax >= 0)
        DrawLine(zeroX, graph->bounds.y, zeroX, graph->bounds.y + graph->bounds.height, DARKGRAY);

    if (graph->yMin <= 0 && graph->yMax >= 0)
        DrawLine(graph->bounds.x, zeroY, graph->bounds.x + graph->bounds.width, zeroY, DARKGRAY);

    float xRange = graph->xMax - graph->xMin;
    float xStep = GetOptimalStep(xRange);
    float xStart = ceil(graph->xMin / xStep) * xStep;

    for (float xVal = xStart; xVal <= graph->xMax; xVal += xStep) {
        int sx = WorldToScreenX(graph, xVal);
        if (sx >= graph->bounds.x && sx <= graph->bounds.x + graph->bounds.width) {
            if (graph->yMin <= 0 && graph->yMax >= 0) {
                DrawLine(sx, zeroY - 5, sx, zeroY + 5, GRAY);
                char valueText[32];
                if (fabs(xVal) < 0.001) xVal = 0;
                snprintf(valueText, sizeof(valueText), "%.1f", xVal);
                int textWidth = MeasureText(valueText, 10);
                DrawText(valueText, sx - textWidth / 2, zeroY + 8, 10, GRAY);
            }
            else {
                DrawLine(sx, graph->bounds.y + graph->bounds.height - 5,
                    sx, graph->bounds.y + graph->bounds.height + 5, GRAY);
                char valueText[32];
                if (fabs(xVal) < 0.001) xVal = 0;
                snprintf(valueText, sizeof(valueText), "%.1f", xVal);
                int textWidth = MeasureText(valueText, 10);
                DrawText(valueText, sx - textWidth / 2,
                    graph->bounds.y + graph->bounds.height + 8, 10, GRAY);
            }
        }
    }

    float yRange = graph->yMax - graph->yMin;
    float yStep = GetOptimalStep(yRange);
    float yStart = ceil(graph->yMin / yStep) * yStep;

    for (float yVal = yStart; yVal <= graph->yMax; yVal += yStep) {
        int sy = WorldToScreenY(graph, yVal);
        if (sy >= graph->bounds.y && sy <= graph->bounds.y + graph->bounds.height) {
            if (graph->xMin <= 0 && graph->xMax >= 0) {
                DrawLine(zeroX - 5, sy, zeroX + 5, sy, GRAY);
                char valueText[32];
                if (fabs(yVal) < 0.001) yVal = 0;
                snprintf(valueText, sizeof(valueText), "%.1f", yVal);
                DrawText(valueText, zeroX - MeasureText(valueText, 10) - 8, sy - 5, 10, GRAY);
            }
            else {
                DrawLine(graph->bounds.x - 5, sy, graph->bounds.x + 5, sy, GRAY);
                char valueText[32];
                if (fabs(yVal) < 0.001) yVal = 0;
                snprintf(valueText, sizeof(valueText), "%.1f", yVal);
                DrawText(valueText, graph->bounds.x - MeasureText(valueText, 10) - 8, sy - 5, 10, GRAY);
            }
        }
    }

    if (graph->xLabel) {
        int labelWidth = MeasureText(graph->xLabel, 14);
        DrawText(graph->xLabel,
            graph->bounds.x + graph->bounds.width - labelWidth - 10,
            graph->bounds.y + graph->bounds.height + 25, 14, BLACK);
    }

    if (graph->yLabel) {
        DrawText(graph->yLabel, graph->bounds.x - 25, graph->bounds.y + 10, 14, BLACK);
    }

    for (int i = 0; i < graph->functionCount; i++) {
        Function f = graph->functions[i];
        Vector2 prev = { 0 };
        bool firstPoint = true;

        float step = (graph->xMax - graph->xMin) / (graph->bounds.width * 2);

        for (float x = graph->xMin; x <= graph->xMax; x += step) {
            float y = f.func(x);

            if (isfinite(y)) {
                int sx = WorldToScreenX(graph, x);
                int sy = WorldToScreenY(graph, y);

                if (sx >= graph->bounds.x - 10 && sx <= graph->bounds.x + graph->bounds.width + 10 &&
                    sy >= graph->bounds.y - 10 && sy <= graph->bounds.y + graph->bounds.height + 10) {

                    if (!firstPoint) {
                        DrawLine(prev.x, prev.y, sx, sy, f.color);
                    }
                    prev = (Vector2){ (float)sx, (float)sy };
                    firstPoint = false;
                }
                else {
                    firstPoint = true;
                }
            }
            else {
                firstPoint = true;
            }
        }
    }

    if (CheckCollisionPointRec(GetMousePosition(), graph->bounds)) {
        Vector2 mp = GetMousePosition();
        float wx = ScreenToWorldX(graph, mp.x);
        float wy = ScreenToWorldY(graph, mp.y);
        DrawCircleV(mp, 3, BLACK);

        char coordText[64];
        snprintf(coordText, sizeof(coordText), "X: %.3f  Y: %.3f", wx, wy);

        int textWidth = MeasureText(coordText, 12);
        int textX = mp.x + 10;
        int textY = mp.y - 20;

        if (textX + textWidth > graph->bounds.x + graph->bounds.width) {
            textX = mp.x - textWidth - 10;
        }
        if (textY < graph->bounds.y) {
            textY = mp.y + 10;
        }

        DrawText(coordText, textX, textY, 12, BLACK);
    }
}

void UnloadGraph(Graph* graph) {
    free(graph->functions);
}