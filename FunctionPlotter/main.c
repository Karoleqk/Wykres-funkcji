#include "raylib.h"
#include "graph.h"
#include "math.h"

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 900

float expFunction(float x) {
    return expf(x);
}

float exponential(float x) {
    return expf(x * 0.5f);
}

float logarithmic(float x) {
    return (x > 0) ? logf(x) : NAN;
}

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Wykres funkcji");

    Graph graphs[3];

    graphs[0] = CreateGraph((Rectangle) { 50, 50, 500, 350 });
    graphs[0].title = "Funkcje trygonometryczne";
    graphs[0].xLabel = "x";
    graphs[0].yLabel = "y";
    AddFunctionToGraph(&graphs[0], sinf, RED);
    AddFunctionToGraph(&graphs[0], cosf, BLUE);

    graphs[1] = CreateGraph((Rectangle) { 650, 50, 500, 350 });
    graphs[1].title = "Wykres Tan(x)";
    graphs[1].xLabel = "x";
    graphs[1].yLabel = "y";
    graphs[1].yMin = -5;
    graphs[1].yMax = 5;
    AddFunctionToGraph(&graphs[1], tanf, GREEN);

    graphs[2] = CreateGraph((Rectangle) { 350, 500, 500, 350 });
    graphs[2].title = "Wykres e^x";
    graphs[2].xLabel = "x";
    graphs[2].yLabel = "y";
    graphs[2].xMin = -5;
    graphs[2].xMax = 5;
    graphs[2].yMin = -2;
    graphs[2].yMax = 10;
    AddFunctionToGraph(&graphs[2], expFunction, PURPLE);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        for (int i = 0; i < 3; i++) {
            UpdateGraph(&graphs[i]);
            DrawGraph(&graphs[i]);
        }

        DrawText("sin(x)", 60, 420, 12, RED);
        DrawText("cos(x)", 120, 420, 12, BLUE);

        EndDrawing();
    }

    for (int i = 0; i < 3; i++) {
        UnloadGraph(&graphs[i]);
    }

    CloseWindow();
    return 0;
}