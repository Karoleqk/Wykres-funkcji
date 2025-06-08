#include "raylib.h"
#include "rlgl.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PI 3.14159265358979323846f

typedef struct{
    int x;
    int y;
} Data;

typedef struct Node{
    Data data;
    struct Node *next;
} Node;

typedef Node *nodePtr;

nodePtr firstNode = NULL;

void append(nodePtr *firstNode, Data data){
    Node *newNode = (Node*)malloc(sizeof(Node));
    newNode->data.x = data.x;
    newNode->data.y = data.y;
    newNode->next = NULL;

    if((*firstNode) == NULL){
        (*firstNode) = newNode;
    } else {
        nodePtr current = (*firstNode);
        while(current->next != NULL) current = current->next;

        current->next = newNode;
    }
}


void freeList(nodePtr firstNode){
    nodePtr tmp;
    while(firstNode != NULL){
        tmp = firstNode;
        firstNode = firstNode->next;
        free(tmp);
    }
}


const int screen_width = 800;
const int screen_height = 450;
const int camera_width = screen_width;
const int camera_height = screen_height;

Camera2D camera = { 0 };


static float Clamp(float value, float min, float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}


int selectedFunction = 0;

void drawFunction(int choice){
    for(int x = 0; x < screen_width - 80; x++){
        float fx1 = (float)x * (4 * PI) / (screen_width - 80);
        float fx2 = (float)(x + 1) * (4 * PI) / (screen_width - 80);

        float fy1;
        float fy2;

        switch(choice){
            case 1:
                fy1 = sinf(fx1);
                fy2 = sinf(fx2);
                break;

            case 2:
                fy1 = cosf(fx1);
                fy2 = cosf(fx2);
                break;
        }

        int screenY1 = screen_height / 2 - (int)(fy1 * 100);
        int screenY2 = screen_height / 2 - (int)(fy2 * 100);

        DrawLine(x + 40, screenY1, x + 41, screenY2, RED);
        DrawLineEx((Vector2){x + 40, screenY1},
                   (Vector2){x + 41, screenY2},
                   3.0f, RED);
    }
}


int main(void){
    InitWindow(screen_width, screen_height, "Wykres sin(x)");

    SetTargetFPS(60);

    // Inicjalizacja kamery
    camera.zoom = 1.0f; // Domyslny zoom

    camera.rotation = 0.0f;

    // Inicjalizacja poruszania sie po planszy
    Vector2 previousMousePosition = {0, 0};
    bool isDragging = false;

    while(!WindowShouldClose()){

        // Zoomowanie
        float wheelMove = GetMouseWheelMove();
        if(wheelMove != 0){
            Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);

            camera.offset = GetMousePosition();

            camera.target = mouseWorldPos;

            float scale = 0.2f * wheelMove;
            camera.zoom = Clamp(expf(logf(camera.zoom) + scale), 0.125f, 64.0f);
        }

        // Poruszanie sie po planszy
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
            previousMousePosition = GetMousePosition();
            isDragging = true;
        }

        if(IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) isDragging = false;

        if(isDragging){
            Vector2 currentMousePosition = GetMousePosition();
            Vector2 delta = {
                previousMousePosition.x - currentMousePosition.x,
                previousMousePosition.y - currentMousePosition.y
            };

            camera.target.x += delta.x / camera.zoom;
            camera.target.y += delta.y / camera.zoom;

            previousMousePosition = currentMousePosition;
        }


        BeginDrawing();

            ClearBackground(RAYWHITE);

            BeginMode2D(camera);

                DrawLine(40, screen_height / 2, screen_width - 40, screen_height / 2, BLACK); // Oœ X
                DrawLine(40, 40, 40, screen_height - 40, BLACK); // OŒ y

                DrawText("x", screen_width - 40, screen_height / 2, 20, BLACK);
                DrawText("y", 20, 40, 20, BLACK);

                if(IsKeyPressed(KEY_ONE)) selectedFunction = 1;
                else if (IsKeyPressed(KEY_TWO)) selectedFunction = 2;

                if(selectedFunction > 0) drawFunction(selectedFunction);

            EndMode2D();

            Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);
            DrawText("1. Sin(x)", 10, 10, 20, BLACK);
            DrawText("2. Cos(x)", 10, 40, 20, BLACK);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
