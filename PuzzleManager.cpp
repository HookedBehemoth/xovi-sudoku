#include "PuzzleManager.hpp"

#include <QRandomGenerator>
#include "Sudoku.hpp"
#include "res/digits.hpp"
#include "rm_SceneLineItem.hpp"

constexpr const float ScreenHeight = 1872.0f;
constexpr const float ScreenCenter = (ScreenHeight / 2.0f);
constexpr const float CellSize = 130.0f;
constexpr const float GridSize = CellSize * 9.0f;
constexpr const float CellRadius = CellSize / 2.0f;
constexpr const float GridStartX = -(CellSize * 4.5f);
constexpr const float GridStartY = (ScreenHeight / 2.0f) - (CellSize * 4.5f);
constexpr const float NumberScale = 40.0f;

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

constexpr auto generateCircle(float radius, Coordinate center, LinePoint* destination, size_t count) {
    for (size_t i = 0; i < count; i++) {
        LinePoint& pt = destination[i];
        float angle = (static_cast<float>(i) / static_cast<float>(count - 1)) * 2.0f * 3.14159265f;
        pt.x = center.x + radius * cos(angle);
        pt.y = center.y + radius * sin(angle);
        pt.speed = 25;
        pt.width = 25;
        pt.direction = 0;
        pt.pressure = 255;
    }
}

constexpr auto sudokuPoints = generateSudokuGrid();

void PuzzleManager::logLine(const Line &line) {
    Line::log(line);
}

Line PuzzleManager::createGrid() {
    return Line::fromPoints(
        std::span<const LinePoint>(sudokuPoints),
        QPointF(0.0f, ScreenCenter), GridSize / 2.0f + 15.0f);
}

Line PuzzleManager::createCircle(const QPointF& _center, float radius) {
    QList<LinePoint> circlePoints(100);

    auto center = Coordinate{static_cast<float>(_center.x()), static_cast<float>(_center.y())};
    generateCircle(radius, center, circlePoints.data(), circlePoints.size());

    return Line::fromPoints(std::move(circlePoints), _center, radius * 1.1);
}

Line PuzzleManager::createLine(const QPointF& start, const QPointF& end) {
    QList<LinePoint> linePoints {
        (LinePoint){static_cast<float>(start.x()), static_cast<float>(start.y()), 25, 25, 0, 255},
        (LinePoint){static_cast<float>(end.x()),   static_cast<float>(end.y()),   25, 25, 0, 255}
    };
    QRectF bounds = {
        std::min(start.x(), end.x()),
        std::min(start.y(), end.y()),
        std::abs(end.x() - start.x()),
        std::abs(end.y() - start.y())
    };

    return Line::fromPoints(std::move(linePoints), bounds);
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

void PuzzleManager::logSceneItems(const QList<std::shared_ptr<SceneItem>>& items) {
    printf("Received %zu scene items\n", items.size());
    for (const auto& itemPtr : items) {
        printf("  SceneItem at %p with refCount %lu\n", itemPtr.get(), itemPtr.use_count());

        auto* item = reinterpret_cast<SceneLineItem*>(itemPtr.get());

        SceneLineItem::log(*item);
    }
}

QList<std::shared_ptr<SceneItem>> PuzzleManager::copyCrosshair() {
    QPointF center(0.0f, 0.0f);
    QList<std::shared_ptr<SceneItem>> itemList(
        {
            std::make_shared<SceneLineItem>(
                SceneLineItem::fromLine(createCircle(center, 50.0f))),
            std::make_shared<SceneLineItem>(
                SceneLineItem::fromLine(createCircle(center, 100.0f))),
            std::make_shared<SceneLineItem>(
                SceneLineItem::fromLine(createCircle(center, 150.0f))),
            std::make_shared<SceneLineItem>(
                SceneLineItem::fromLine(createCircle(center, 200.0f))),
            std::make_shared<SceneLineItem>(
                SceneLineItem::fromLine(createLine(QPointF(-200.0f, 0.0f), QPointF(200.0f, 0.0f)))),
            std::make_shared<SceneLineItem>(
                SceneLineItem::fromLine(createLine(QPointF(0.0f, -200.0f), QPointF(0.0f, 200.0f)))),
        }
    );

    return itemList;
}

Line PuzzleManager::createStar(const QPointF& center, double size, size_t points) {
    size_t segments = points * 2;
    QList<LinePoint> starPoints(segments + 1);

    for (size_t i = 0; i < segments + 1; i++) {
        const double angle = (static_cast<double>(i) / static_cast<double>(segments)) * 2.0 * 3.14159265;
        const double radius = (i % 2 == 0) ? size : (size / 2.0);
        starPoints[i] = (LinePoint){
            static_cast<float>(center.x() + radius * cos(angle)),
            static_cast<float>(center.y() + radius * sin(angle)),
            25, 25, 0, 255};
    }

    return Line::fromPoints(std::move(starPoints), center, size);
}

QList<std::shared_ptr<SceneItem>> PuzzleManager::copyStars(size_t count, double spread, size_t points) {
    auto random = QRandomGenerator::global();

    QList<std::shared_ptr<SceneItem>> itemList(count);

    for (size_t i = 0; i < count; ++i) {
        const double size = random->bounded(60.0);
        const double x = random->bounded(spread - size) + size;
        const double y = random->bounded(spread - size) + size;
        itemList[i] = std::make_shared<SceneLineItem>(
            SceneLineItem::fromLine(
                createStar(QPointF(x, y), size, points))
        );
    }

    return itemList;
}

void PuzzleManager::sleepMs(int ms) {
    ::usleep(ms * 1000);
}

bool PuzzleManager::setupVtablePtr(const QList<std::shared_ptr<SceneItem>>& items) {
    if (items.empty()) {
        printf("setupVtablePtr: no items provided\n");
        return false;
    }
    auto* item = reinterpret_cast<SceneLineItem*>(items.first().get());
    SceneLineItem::setupVtable(item->vtable);
    return true;
}