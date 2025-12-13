#include <QQmlApplicationEngine>
#include "PuzzleManager.hpp"

extern "C" void registerQmldiff() {
    qmlRegisterSingletonInstance<PuzzleManager>(
        "net.sudoku", 1, 0, "PuzzleManager", new PuzzleManager());
}
