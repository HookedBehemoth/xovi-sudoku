#pragma once

#include <QList>
#include <QObject>
#include <QPointF>
#include <QVariant>
#include "Sudoku.hpp"
#include "rm_Line.hpp"
#include "rm_SceneItem.hpp"

class PuzzleManager : public QObject
{
    Q_OBJECT
public:
    explicit PuzzleManager(QObject *parent = nullptr) : QObject(parent) {}

    Q_INVOKABLE void logLine(const Line &line);

    Q_INVOKABLE Line createGrid();
    Q_INVOKABLE Line createCircle(const QPointF& center, float radius);
    Q_INVOKABLE Line createLine(const QPointF& start, const QPointF& end);

    Q_INVOKABLE QVariant getSudoku(int level);
    Q_INVOKABLE QVariant getSudokuNumber(
        const Sudoku& sudoku,
        int column, int row,
        bool maskHint);
    Q_INVOKABLE QVariant getNumber(int number, const QPointF& center, float scale);

    Q_INVOKABLE void logSceneItems(const QList<std::shared_ptr<SceneItem>>& items);
    Q_INVOKABLE QList<std::shared_ptr<SceneItem>> copyCrosshair();

    Q_INVOKABLE Line createStar(const QPointF& center, double size, size_t points = 5);
    Q_INVOKABLE QList<std::shared_ptr<SceneItem>> copyStars(size_t count, double spread, size_t points = 5);
};
