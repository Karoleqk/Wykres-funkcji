#ifndef MATH_FUNCTIONS_H
#define MATH_FUNCTIONS_H

#include "raylib.h"
#include <stdbool.h>
#include <math.h>

#define PI 3.14159265358979323846f
#define MAX_FUNCTIONS 16

typedef enum {
    FUNC_NONE = 0,
    FUNC_SIN,
    FUNC_COS,
    FUNC_TAN,
    FUNC_EXP,
    FUNC_LOG,
    FUNC_SQRT,
    FUNC_X,
    FUNC_X2,
    FUNC_X3,
    FUNC_ABS,
    FUNC_SINH,
    FUNC_COSH,
    FUNC_ASIN,
    FUNC_ACOS,
    FUNC_ATAN,
    FUNC_COUNT
} FunctionType;

typedef struct {
    FunctionType type;
    Color color;
    bool visible;
    float amplitude;
    float frequency;
    float phase;
    float offset;
    float lineWidth;
    char name[32];
} MathFunction;

typedef struct FunctionNode {
    MathFunction function;
    struct FunctionNode* next;
} FunctionNode;

typedef struct {
    FunctionNode* first;
    int count;
} FunctionList;

void InitFunctionList(FunctionList* list);
void AddFunction(FunctionList* list, FunctionType type);
void RemoveFunction(FunctionList* list, FunctionType type);
void ToggleFunction(FunctionList* list, FunctionType type);
void ClearFunctionList(FunctionList* list);
bool HasFunction(FunctionList* list, FunctionType type);
MathFunction* GetFunction(FunctionList* list, FunctionType type);

float EvaluateFunction(const MathFunction* func, float x);
bool IsValidFunctionValue(float value);

const char* GetFunctionName(FunctionType type);
Color GetDefaultFunctionColor(FunctionType type);
void SetFunctionParameters(MathFunction* func, float amplitude, float frequency, float phase, float offset);

void DrawFunctionInRange(const MathFunction* func, float xStart, float xEnd, float yMin, float yMax, Rectangle bounds);

void GetFunctionBounds(const MathFunction* func, float xMin, float xMax, float* yMin, float* yMax);
bool IsFunctionContinuous(FunctionType type);

#endif