# Define the project name
TEMPLATE = lib
TARGET = sudoku
CONFIG += shared plugin no_plugin_name_prefix
QMAKE_LFLAGS += -Wl,--no-undefined

xoviextension.target = xovi.c
xoviextension.commands = python3 $$(XOVI_REPO)/util/xovigen.py -o xovi.c -H xovi.h sudoku.xovi
xoviextension.depends = sudoku.xovi sudoku.qmd strokehook.qmd

QMAKE_EXTRA_TARGETS += xoviextension
PRE_TARGETDEPS += xovi.c

# Define the Qt modules required
QT += quick qml

# Define the C++ standard version
CONFIG += c++20

# Specify the source files
SOURCES += \
    main.cpp entry.c xovi.c \
    PuzzleManager.cpp Sudoku.cpp \
    rm_Line.cpp rm_SceneLineItem.cpp

HEADERS += PuzzleManager.hpp Sudoku.hpp

QMAKE_CXXFLAGS += -fPIC -Werror

# QMAKE_CXX = aarch64-remarkable-linux-g++

RESOURCES += sudoku.qrc
