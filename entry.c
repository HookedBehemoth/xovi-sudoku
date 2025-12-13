#define _GNU_SOURCE
#include <stdio.h>

#include "xovi.h"

void registerQmldiff();

#define qmldiff_add_external_diff ((unsigned long long int (*)(const char*, const char*)) LINKTABLEVALUES[2])

void _xovi_construct() {
    printf("Registering PuzzleManager\n");
    Environment->requireExtension("qt-resource-rebuilder", 0, 2, 0);
    registerQmldiff();
    qmldiff_add_external_diff(r$toolbar, "Logging of line strokes");
#ifdef DEBUG
    qmldiff_add_external_diff(r$strokehook, "Line stroke logging");
#endif
}
