#include "math_functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void InitFunctionList(FunctionList* list) {
    list->first = NULL;
    list->count = 0;
}

void AddFunction(FunctionList* list, FunctionType type) {
    if (HasFunction(list, type)) return;

    FunctionNode* newNode = (FunctionNode*)malloc(sizeof(FunctionNode));
    if (!newNode) return;

    newNode->function.type = type;
    newNode->function.color = GetDefaultFunctionColor(type);
    newNode->function.visible = true;
    newNode->function.amplitude = 1.0f;
    newNode->function.frequency = 1.0f;
    newNode->function.phase = 0.0f;
    newNode->function.offset = 0.0f;
    newNode->function.lineWidth = 2.0f;
    strcpy(newNode->function.name, GetFunctionName(type));

    newNode->next = list->first;
    list->first = newNode;
    list->count++;
}

void RemoveFunction(FunctionList* list, FunctionType type) {
    if (!list->first) return;

    if (list->first->function.type == type) {
        FunctionNode* temp = list->first;
        list->first = list->first->next;
        free(temp);
        list->count--;
        return;
    }

    FunctionNode* current = list->first;
    while (current->next) {
        if (current->next->function.type == type) {
            FunctionNode* temp = current->next;
            current->next = temp->next;
            free(temp);
            list->count--;
            return;
        }
        current = current->next;
    }
}

void ToggleFunction(FunctionList* list, FunctionType type) {
    if (HasFunction(list, type)) {
        RemoveFunction(list, type);
    }
    else {
        AddFunction(list, type);
    }
}

void ClearFunctionList(FunctionList* list) {
    while (list->first) {
        FunctionNode* temp = list->first;
        list->first = list->first->next;
        free(temp);
    }
    list->count = 0;
}

bool HasFunction(FunctionList* list, FunctionType type) {
    FunctionNode* current = list->first;
    while (current) {
        if (current->function.type == type) {
            return true;
        }
        current = current->next;
    }
    return false;
}

MathFunction* GetFunction(FunctionList* list, FunctionType type) {
    FunctionNode* current = list->first;
    while (current) {
        if (current->function.type == type) {
            return &current->function;
        }
        current = current->next;
    }
    return NULL;
}

float EvaluateFunction(const MathFunction* func, float x) {
    float scaledX = func->frequency * x + func->phase;
    float result;

    switch (func->type) {
    case FUNC_SIN:
        result = func->amplitude * sinf(scaledX) + func->offset;
        break;
    case FUNC_COS:
        result = func->amplitude * cosf(scaledX) + func->offset;
        break;
    case FUNC_TAN:
        result = func->amplitude * tanf(scaledX) + func->offset;
        break;
    case FUNC_EXP:
        if (scaledX > 20.0f) return INFINITY; // Prevent overflow
        result = func->amplitude * expf(scaledX) + func->offset;
        break;
    case FUNC_LOG:
        if (scaledX <= 0) return -INFINITY;
        result = func->amplitude * logf(scaledX) + func->offset;
        break;
    case FUNC_SQRT:
        if (scaledX < 0) return NAN;
        result = func->amplitude * sqrtf(scaledX) + func->offset;
        break;
    case FUNC_X:
        result = func->amplitude * scaledX + func->offset;
        break;
    case FUNC_X2:
        result = func->amplitude * scaledX * scaledX + func->offset;
        break;
    case FUNC_X3:
        result = func->amplitude * scaledX * scaledX * scaledX + func->offset;
        break;
    case FUNC_ABS:
        result = func->amplitude * fabsf(scaledX) + func->offset;
        break;
    case FUNC_SINH:
        if (fabsf(scaledX) > 20.0f) return scaledX > 0 ? INFINITY : -INFINITY;
        result = func->amplitude * sinhf(scaledX) + func->offset;
        break;
    case FUNC_COSH:
        if (fabsf(scaledX) > 20.0f) return INFINITY;
        result = func->amplitude * coshf(scaledX) + func->offset;
        break;
    case FUNC_ASIN:
        if (fabsf(scaledX) > 1.0f) return NAN;
        result = func->amplitude * asinf(scaledX) + func->offset;
        break;
    case FUNC_ACOS:
        if (fabsf(scaledX) > 1.0f) return NAN;
        result = func->amplitude * acosf(scaledX) + func->offset;
        break;
    case FUNC_ATAN:
        result = func->amplitude * atanf(scaledX) + func->offset;
        break;
    default:
        result = 0.0f;
    }

    return result;
}

bool IsValidFunctionValue(float value) {
    return !isnan(value) && !isinf(value);
}

const char* GetFunctionName(FunctionType type) {
    switch (type) {
    case FUNC_SIN: return "sin(x)";
    case FUNC_COS: return "cos(x)";
    case FUNC_TAN: return "tan(x)";
    case FUNC_EXP: return "e^x";
    case FUNC_LOG: return "ln(x)";
    case FUNC_SQRT: return "√x";
    case FUNC_X: return "x";
    case FUNC_X2: return "x²";
    case FUNC_X3: return "x³";
    case FUNC_ABS: return "|x|";
    case FUNC_SINH: return "sinh(x)";
    case FUNC_COSH: return "cosh(x)";
    case FUNC_ASIN: return "arcsin(x)";
    case FUNC_ACOS: return "arccos(x)";
    case FUNC_ATAN: return "arctan(x)";
    default: return "unknown";
    }
}

Color GetDefaultFunctionColor(FunctionType type) {
    switch (type) {
    case FUNC_SIN: return RED;
    case FUNC_COS: return BLUE;
    case FUNC_TAN: return GREEN;
    case FUNC_EXP: return ORANGE;
    case FUNC_LOG: return PURPLE;
    case FUNC_SQRT: return BROWN;
    case FUNC_X: return BLACK;
    case FUNC_X2: return DARKBLUE;
    case FUNC_X3: return DARKGREEN;
    case FUNC_ABS: return MAROON;
    case FUNC_SINH: return PINK;
    case FUNC_COSH: return LIME;
    case FUNC_ASIN: return DARKPURPLE;
    case FUNC_ACOS: return DARKBROWN;
    case FUNC_ATAN: return GOLD;
    default: return GRAY;
    }
}

void SetFunctionParameters(MathFunction* func, float amplitude, float frequency, float phase, float offset) {
    func->amplitude = amplitude;
    func->frequency = frequency;
    func->phase = phase;
    func->offset = offset;
}

void DrawFunctionInRange(const MathFunction* func, float xStart, float xEnd, float yMin, float yMax, Rectangle bounds) {
    if (!func->visible) return;

    int steps = (int)(bounds.width * 2); // High resolution for smooth curves
    float stepSize = (xEnd - xStart) / steps;

    Vector2 prevPoint = { -1, -1 };
    bool prevValid = false;

    for (int i = 0; i <= steps; i++) {
        float x = xStart + i * stepSize;
        float y = EvaluateFunction(func, x);

        if (IsValidFunctionValue(y) && y >= yMin && y <= yMax) {
            // Transform to screen coordinates
            float screenX = bounds.x + ((x - xStart) / (xEnd - xStart)) * bounds.width;
            float screenY = bounds.y + bounds.height - ((y - yMin) / (yMax - yMin)) * bounds.height;

            Vector2 currentPoint = { screenX, screenY };

            if (prevValid &&
                screenX >= bounds.x && screenX <= bounds.x + bounds.width &&
                screenY >= bounds.y && screenY <= bounds.y + bounds.height) {

                // Check for discontinuities (like in tan function)
                float distance = Vector2Distance(prevPoint, currentPoint);
                if (distance < bounds.height * 0.5f) { // Reasonable threshold
                    DrawLineEx(prevPoint, currentPoint, func->lineWidth, func->color);
                }
            }

            prevPoint = currentPoint;
            prevValid = true;
        }
        else {
            prevValid = false;
        }
    }
}

void GetFunctionBounds(const MathFunction* func, float xMin, float xMax, float* yMin, float* yMax) {
    *yMin = INFINITY;
    *yMax = -INFINITY;

    int samples = 1000;
    float stepSize = (xMax - xMin) / samples;

    for (int i = 0; i <= samples; i++) {
        float x = xMin + i * stepSize;
        float y = EvaluateFunction(func, x);

        if (IsValidFunctionValue(y)) {
            if (y < *yMin) *yMin = y;
            if (y > *yMax) *yMax = y;
        }
    }

    // If no valid values found, use default range
    if (*yMin == INFINITY || *yMax == -INFINITY) {
        *yMin = -10.0f;
        *yMax = 10.0f;
    }
}

bool IsFunctionContinuous(FunctionType type) {
    switch (type) {
    case FUNC_TAN:
    case FUNC_LOG:
    case FUNC_SQRT:
    case FUNC_ASIN:
    case FUNC_ACOS:
        return false;
    default:
        return true;
    }
}