#ifndef GRAPH_H
#define GRAPH_H

#include "raylib.h"
#include "raymath.h"

typedef float (*FunctionPtr)(float);

typedef struct {
    FunctionPtr func;
    Color color;
} Function;

typedef struct {
    Rectangle bounds;
    float xMin, xMax, yMin, yMax;
    float scaleX, scaleY;
    int functionCount;
    Function* functions;
    bool dragging;
    Vector2 dragStart;

    const char* xLabel;
    const char* yLabel;
    const char* title;
} Graph;

Graph CreateGraph(Rectangle bounds);
void AddFunctionToGraph(Graph* graph, FunctionPtr func, Color color);
void UpdateGraph(Graph* graph);
void DrawGraph(Graph* graph);
void UnloadGraph(Graph* graph);

#endif
