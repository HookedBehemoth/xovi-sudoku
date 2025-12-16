# Define the project name
TEMPLATE = lib
TARGET = sudoku
CONFIG += shared plugin no_plugin_name_prefix
QMAKE_LFLAGS += -Wl,--no-undefined

# Configure build directories
OBJECTS_DIR = build/obj
MOC_DIR = build/moc
RCC_DIR = build/rcc
UI_DIR = build/ui
XOVI_DIR = build/xovi

xoviextension.target = build/xovi/xovi.c
xoviextension.commands = mkdir -p $$XOVI_DIR && python3 $$(XOVI_REPO)/util/xovigen.py -o $$XOVI_DIR/xovi.c -H $$XOVI_DIR/xovi.h sudoku.xovi
xoviextension.depends = sudoku.xovi sudoku.qmd strokehook.qmd

QMAKE_EXTRA_TARGETS += xoviextension
PRE_TARGETDEPS += $$XOVI_DIR/xovi.c

# Define the Qt modules required
QT += quick qml

# Define the C++ standard version
CONFIG += c++20

# Specify the source files
SOURCES += \
    main.cpp entry.c $$XOVI_DIR/xovi.c \
    PuzzleManager.cpp Sudoku.cpp \
    rm_Line.cpp rm_SceneLineItem.cpp

HEADERS += PuzzleManager.hpp Sudoku.hpp
INCLUDEPATH += $$XOVI_DIR

QMAKE_CXXFLAGS += -fPIC -Werror

# QMAKE_CXX = aarch64-remarkable-linux-g++

RESOURCES += sudoku.qrc
