#include "PuzzleManager.hpp"

#include "Sudoku.hpp"
#include "res/digits.hpp"

constexpr const float ScreenHeight = 1696.0f;
constexpr const float ScreenCenter = (ScreenHeight / 2.0f);
constexpr const float CellSize = 90.0f;
constexpr const float GridSize = CellSize * 9.0f;
constexpr const float CellRadius = CellSize / 2.0f;
constexpr const float GridStartX = -(CellSize * 4.5f);
constexpr const float GridStartY = (ScreenHeight / 2.0f) - (CellSize * 4.5f);
constexpr const float NumberScale = 28.0f;

constexpr auto generateSudokuGrid()
{
    constexpr const float GridEndX = GridStartX + GridSize;
    constexpr const float GridEndY = GridStartY + GridSize;

    std::array<LinePoint, 40> pts{};

    std::size_t i = 0;

    // ---- HORIZONTAL LINES (snake pattern) ----
    for (int y = 0; y < 10; y++) {
        float yy = GridStartY + y * CellSize;
        unsigned char width = (y % 3 == 0) ? 25 : 10;

        if (y % 2 == 0) {
            pts[i++] = (LinePoint){GridStartX, yy, 25, width, 0, 255};
            pts[i++] = (LinePoint){GridEndX,   yy, 25, width, 0, 255};
        } else {
            pts[i++] = (LinePoint){GridEndX,   yy, 25, width, 0, 255};
            pts[i++] = (LinePoint){GridStartX, yy, 25, width, 0, 255};
        }
    }

    // ---- VERTICAL LINES (snake pattern) ----
    for (int x = 0; x < 10; x++) {
        float xx = GridStartX + x * CellSize;
        unsigned char width = (x % 3 == 0) ? 25 : 10;

        if (x % 2 == 0) {
            pts[i++] = (LinePoint){xx, GridEndY,   25, width, 0, 255};
            pts[i++] = (LinePoint){xx, GridStartY, 25, width, 0, 255};
        } else {
            pts[i++] = (LinePoint){xx, GridStartY, 25, width, 0, 255};
            pts[i++] = (LinePoint){xx, GridEndY,   25, width, 0, 255};
        }
    }

    return pts;
}

constexpr auto sudokuPoints = generateSudokuGrid();

Line PuzzleManager::createGrid() {
    return Line::fromPoints(
        std::span<const LinePoint>(sudokuPoints),
        QPointF(0.0f, ScreenCenter), GridSize / 2.0f + 15.0f);
}

QVariant PuzzleManager::getSudoku(int level) {
    auto difficulty = static_cast<Sudoku::Difficulty>(level);
    auto sudokuOpt = Sudoku::loadFromResource(difficulty, std::nullopt);
    if (!sudokuOpt.has_value()) {
        return QVariant();
    }

    return QVariant::fromValue(sudokuOpt.value());
} 

QVariant PuzzleManager::getSudokuNumber(
    const Sudoku& sudoku,
    int column, int row,
    bool maskHint) {
    if (row < 0 || row >= 9 || column < 0 || column >= 9) {
        return QVariant();
    }

    int number = sudoku.Number[row * 9 + column];
    if (number < 1 || number > 9) {
        return QVariant();
    }

    if (maskHint && !sudoku.HintMask[row * 9 + column]) {
        return QVariant();
    }

    QPointF center = {
        GridStartX + (column + 0.5f) * CellSize,
        GridStartY + (row + 0.5f) * CellSize
    };

    return getNumber(number, center, NumberScale);
}

QVariant PuzzleManager::getNumber(int number, const QPointF& center, float scale) {
    auto points = DigitPoints[number - 1];
    auto pointCount = points.size();

    QList<LinePoint> linePoints(pointCount);

    for (size_t i = 0; i < pointCount; i++) {
        linePoints[i] = (LinePoint){
            static_cast<float>(center.x()) + points[i].x *  scale,
            static_cast<float>(center.y()) + points[i].y * -scale,
            25, 25, 0, 255};
    }

    auto line = Line::fromPoints(std::move(linePoints), center, scale);
    return QVariant::fromValue(std::move(line));
}
