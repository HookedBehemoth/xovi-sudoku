#define _GNU_SOURCE
#include <stdio.h>

#include "xovi.h"

void registerQmldiff();

void _xovi_construct() {
    printf("Registering PuzzleManager\n");
    Environment->requireExtension("qt-resource-rebuilder", 0, 2, 0);
    registerQmldiff();
    qt_resource_rebuilder$qmldiff_add_external_diff(r$toolbar, "Logging of line strokes");
#ifdef DEBUG
    qt_resource_rebuilder$qmldiff_add_external_diff(r$strokehook, "Line stroke logging");
#endif
}
