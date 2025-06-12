#include "raylib.h"
#include "rlgl.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PI 3.14159265358979323846f

typedef struct Node{
    int functionId;
    struct Node *next;
} Node;

typedef Node *nodePtr;

nodePtr firstNode = NULL;

int selectedFunction;

void append(nodePtr *firstNode, int functionId){
    Node *newNode = (Node*)malloc(sizeof(Node));
    newNode->functionId = functionId;
    newNode->next = NULL;

    if((*firstNode) == NULL){
        (*firstNode) = newNode;
    } else {
        nodePtr current = (*firstNode);
        while(current->next != NULL) current = current->next;

        current->next = newNode;
    }
}

bool isRemoved(nodePtr *firstNode, int functionId){
    if((*firstNode) == NULL) return false;

    if((*firstNode)->functionId == functionId){
        nodePtr tmp = (*firstNode);
        (*firstNode) = (*firstNode)->next;
        free(tmp);
        return true;
    }

    nodePtr current = (*firstNode);
    while(current->next != NULL){
        if(current->next->functionId == functionId){
            nodePtr tmp = current->next;
            current->next = tmp->next;
            free(tmp);
            return true;
        }
        current = current->next;
    }

    return false;
}


void freeList(nodePtr firstNode){
    nodePtr tmp;
    while(firstNode != NULL){
        tmp = firstNode;
        firstNode = firstNode->next;
        free(tmp);
    }
}


const int screen_width = 1100;
const int screen_height = 550;
const int camera_width = screen_width;
const int camera_height = screen_height;

Camera2D camera = { 0 };


static float Clamp(float value, float min, float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}


void drawFunctions(nodePtr firstNode){
    Color color = RED;

    while(firstNode != NULL){
        for(int x = -screen_width; x < screen_width; x++){
            float fx1 = ((float)x / screen_width) * 8 * PI - 4 * PI;
            float fx2 = ((float)(x + 1) / screen_width) * 8 * PI - 4 * PI;

            float fy1;
            float fy2;


            // Select function
            switch(firstNode->functionId){
                case 1:
                    fy1 = sinf(fx1);
                    fy2 = sinf(fx2);
                    color = RED;
                    break;

                case 2:
                    fy1 = cosf(fx1);
                    fy2 = cosf(fx2);
                    color = BLUE;
                    break;

                case 3:
                    fy1 = exp(fx1);
                    fy2 = exp(fx2);
                    color = PINK;
                    break;

                case 4:
                    fy1 = tan(fx1);
                    fy2 = tan(fx2);
                    color = MAROON;
                    break;

                default:
                    continue;
            }

            int screenY1 = screen_height / 2 - (int)(fy1 * 100);
            int screenY2 = screen_height / 2 - (int)(fy2 * 100);

            DrawLineEx((Vector2){x, screenY1},
                       (Vector2){x + 1, screenY2},
                       3.0f, color);
        }
        firstNode = firstNode->next;
    }


}


int main(void){
    InitWindow(screen_width, screen_height, "Wykres sin(x)");

    SetTargetFPS(60);

    // Inicjalizacja kamery
    camera.zoom = 1.0f; // Domyslny zoom
    camera.target.x = -(screen_width / 2);
    camera.target.y = 0;
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

                DrawLine(-screen_width, screen_height / 2, screen_width, screen_height / 2, BLACK); // Oœ X
                DrawLine(0, 40, 0, screen_height - 40, BLACK); // OŒ y

                // Podpisy osi
                DrawText("x", screen_width - 40, screen_height / 2, 20, BLACK);
                DrawText("y", 20, 40, 20, BLACK);

                // Wyrysuj podpisy osi OX
                DrawText("-4 pi", -1110, screen_height / 2 + 15, 14, BLACK);
                DrawText("-3 pi", -835, screen_height / 2 + 15, 14, BLACK);
                DrawText("-2 pi", -560, screen_height / 2 + 15, 14, BLACK);
                DrawText("-1 pi", -285, screen_height / 2 + 15, 14, BLACK);
                DrawText("0 pi", -10, screen_height / 2 + 15, 14, BLACK);
                DrawText("1 pi", 265, screen_height / 2 + 15, 14, BLACK);
                DrawText("2 pi", 540, screen_height / 2 + 15, 14, BLACK);
                DrawText("3 pi", 815, screen_height / 2 + 15, 14, BLACK);
                DrawText("4 pi", 1090, screen_height / 2 + 15, 14, BLACK);

                // Wyrysuj podpisy osi OY
                DrawLineEx((Vector2){-8, screen_height / 2 + 100},
                           (Vector2){8, screen_height / 2 + 100},
                           1.5f, BLACK);

                DrawLineEx((Vector2){-8, screen_height / 2 - 100},
                           (Vector2){8, screen_height / 2 - 100},
                           1.5f, BLACK);

                DrawText("1", -15, screen_height / 2 - 105, 14, BLACK);
                DrawText("-1", -22, screen_height / 2 + 95, 14, BLACK);

                if(IsKeyPressed(KEY_ONE)) selectedFunction = 1;
                else if (IsKeyPressed(KEY_TWO)) selectedFunction = 2;
                else if (IsKeyPressed(KEY_THREE)) selectedFunction = 3;
                else if (IsKeyPressed(KEY_FOUR)) selectedFunction = 4;

                if(selectedFunction > 0) {
                    if(!isRemoved(&firstNode, selectedFunction)){
                        append(&firstNode, selectedFunction);
                    }

                    selectedFunction = 0;
                }

                drawFunctions(firstNode);

                for(int x = -screen_width; x <= screen_width; x += screen_width / 4){
                    DrawLineEx((Vector2){x, screen_height / 2 + 10},
                           (Vector2){x, screen_height / 2 - 10},
                           2.0f, BLACK);
                }

            EndMode2D();

            // Wyrysuj menu
            Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);
            DrawText("1. Sin(x)", 10, 10, 20, BLACK);
            DrawText("2. Cos(x)", 10, 40, 20, BLACK);
            DrawText("3. e^x", 10, 70, 20, BLACK);
            DrawText("4. Tan(x)", 10, 100, 20, BLACK);

            DrawText(TextFormat("X: %.1f, Y: %.1f", GetMousePosition().x, GetMousePosition().y), 300, 10, 20, BLACK);

        EndDrawing();
    }

    freeList(firstNode);

    CloseWindow();

    return 0;
}
