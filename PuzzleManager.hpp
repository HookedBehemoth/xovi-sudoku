#pragma once

#include <QObject>
#include <QPointF>
#include <QVariant>
#include "Sudoku.hpp"
#include "rm_Line.hpp"

class PuzzleManager : public QObject
{
    Q_OBJECT
public:
    explicit PuzzleManager(QObject *parent = nullptr) : QObject(parent) {}

    Q_INVOKABLE Line createGrid();

    Q_INVOKABLE QVariant getSudoku(int level);
    Q_INVOKABLE QVariant getSudokuNumber(
        const Sudoku& sudoku,
        int column, int row,
        bool maskHint);
    Q_INVOKABLE QVariant getNumber(int number, const QPointF& center, float scale);
};
