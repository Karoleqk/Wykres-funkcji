#ifndef UTILS_H
#define UTILS_H

#include "raylib.h"
#include "graph.h"

int WorldToScreenX(Graph* graph, float x);
int WorldToScreenY(Graph* graph, float y);
float ScreenToWorldX(Graph* graph, int x);
float ScreenToWorldY(Graph* graph, int y);

#endif
