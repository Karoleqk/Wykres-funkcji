#include "utils.h"

int WorldToScreenX(Graph* graph, float x) {
    return graph->bounds.x + ((x - graph->xMin) / (graph->xMax - graph->xMin)) * graph->bounds.width;
}

int WorldToScreenY(Graph* graph, float y) {
    return graph->bounds.y + graph->bounds.height - ((y - graph->yMin) / (graph->yMax - graph->yMin)) * graph->bounds.height;
}

float ScreenToWorldX(Graph* graph, int x) {
    return graph->xMin + ((float)(x - graph->bounds.x) / graph->bounds.width) * (graph->xMax - graph->xMin);
}

float ScreenToWorldY(Graph* graph, int y) {
    return graph->yMin + (1.0f - (float)(y - graph->bounds.y) / graph->bounds.height) * (graph->yMax - graph->yMin);
}
