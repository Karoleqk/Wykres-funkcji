#include "function_plotter.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    if (!InitializePlotter()) {
        fprintf(stderr, "Failed to initialize Function Plotter\n");
        return -1;
    }

    RunPlotter();
    ShutdownPlotter();

    return 0;
}