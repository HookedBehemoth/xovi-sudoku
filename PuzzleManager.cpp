#include "PuzzleManager.hpp"
#include "Sudoku.hpp"
#include "Numbers.hpp"

struct LinePoint {
    float x;
    float y;
    unsigned short speed;
    unsigned short width;
    unsigned char direction;
    unsigned char pressure;
} __attribute__((packed));
static_assert(sizeof(LinePoint) == 0xe, "LinePoint size mismatch");

constexpr const float ScreenHeight = 1872.0f;
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

constexpr auto sudokuPoints = generateSudokuGrid();

void PuzzleManager::line(const Line &line) {
#ifdef DEBUG
    printf("Line log - tool: %d, color: %d, rgba: %08X, pointRc: %08X, pointCount: %u, maskScale: %f, thickness: %f, unk_x24: %08X\n",
        line.tool, line.color, line.rgba, line.pointRc, line.pointCount, line.maskScale, line.thickness, line.unk_x24);
    printf("Line log - bounds: (%.2f, %.2f, %.2f, %.2f)\n", line.bounds.x, line.bounds.y, line.bounds.width, line.bounds.height);

    printf("Line points:\n");
    for (unsigned int i = 0; i < line.pointCount; i++) {
        const LinePoint &point = line.points[i];
        printf("  Point %u: (x: %f, y: %f, speed: %u, width: %u, direction: %u, pressure: %u)\n",
            i, point.x, point.y, point.speed, point.width, point.direction, point.pressure);
    }
#else
    (void)line;
#endif
}

Line PuzzleManager::createGrid() {
    Line gridLine = {};
    gridLine.tool = 0x13; // SolidPen
    gridLine.points = sudokuPoints.data();
    gridLine.pointCount = sudokuPoints.size();
    gridLine.color = 0; // Black
    gridLine.rgba = 0xff000000;
    gridLine.thickness = 0.0f;
    gridLine.maskScale = 1.0;
    gridLine.bounds.x = GridStartX - 15.0f;
    gridLine.bounds.y = GridStartY - 15.0f;
    gridLine.bounds.width = GridSize + 30.0f;
    gridLine.bounds.height = GridSize + 30.0f;

    return gridLine;
}

Line PuzzleManager::createCircle() {
    static std::array<LinePoint, 100> circlePoints{};
    static bool initialized = false;
    if (!initialized) {
        const float centerX = 0.0f;
        const float centerY = ScreenHeight / 2.0f;
        const float radius = 400.0f;
        for (size_t i = 0; i < circlePoints.size(); i++) {
            float angle = (static_cast<float>(i) / static_cast<float>(circlePoints.size() - 1)) * 2.0f * 3.14159265f;
            circlePoints[i].x = centerX + radius * cos(angle);
            circlePoints[i].y = centerY + radius * sin(angle);
            circlePoints[i].speed = 25;
            circlePoints[i].width = 25;
            circlePoints[i].direction = 0;
            circlePoints[i].pressure = 255;
        }
        initialized = true;
    }

    Line circleLine = {};
    circleLine.tool = 0x13; // SolidPen
    circleLine.points = circlePoints.data();
    circleLine.pointCount = circlePoints.size();
    circleLine.color = 0; // Black
    circleLine.rgba = 0xff000000;
    circleLine.thickness = 0.0f;
    circleLine.maskScale = 1.0;
    circleLine.bounds.x = -400.0;
    circleLine.bounds.y = 560.0;
    circleLine.bounds.width = 800.0;
    circleLine.bounds.height = 800.0;

    return circleLine;
}

Line PuzzleManager::createLine(const QPointF& start, const QPointF& end) {
    static std::array<LinePoint, 2> linePoints{};

    linePoints[0] = (LinePoint){static_cast<float>(start.x()), static_cast<float>(start.y()), 25, 25, 0, 255};
    linePoints[1] = (LinePoint){static_cast<float>(end.x()),   static_cast<float>(end.y()),   25, 25, 0, 255};

    Line line = {};
    line.tool = 0x13; // SolidPen
    line.points = linePoints.data();
    line.pointCount = linePoints.size();
    line.color = 0; // Black
    line.rgba = 0xff000000;
    line.thickness = 0.0f;
    line.maskScale = 1.0;
    line.bounds.x = std::min(start.x(), end.x());
    line.bounds.y = std::min(start.y(), end.y());
    line.bounds.width = std::abs(end.x() - start.x());
    line.bounds.height = std::abs(end.y() - start.y());

    return line;
}

QVariant PuzzleManager::getSudoku(int level) {
    auto difficulty = static_cast<Sudoku::Difficulty>(level);
    auto sudokuOpt = Sudoku::loadFromResource(difficulty, std::nullopt);
    if (!sudokuOpt.has_value()) {
        return QVariant();
    }

    // arbitrary delay as we are NOT thread save
    usleep(50'000);
    return QVariant::fromValue(sudokuOpt.value());
}

QVariant PuzzleManager::getNumber(
    const Sudoku& sudoku,
    int column, int row) {
    if (row < 0 || row >= 9 || column < 0 || column >= 9) {
        return QVariant();
    }

    int number = sudoku.Number[row * 9 + column];
    if (number < 1 || number > 9) {
        return QVariant();
    }

    if (!sudoku.HintMask[row * 9 + column]) {
        return QVariant();
    }

    usleep(1000);
    QPointF center = QPointF(
        GridStartX + (column + 0.5f) * CellSize,
        GridStartY + (row + 0.5f) * CellSize
    );

    Line line = {};
    line.tool = 0x13; // SolidPen
    line.color = 0; // Black
    line.rgba = 0xff000000;
    line.thickness = 0.0f;
    line.maskScale = 1.0;

    const QPointF* points = nullptr;
    size_t pointCount = 0;
    switch (number) {
        case 1:
            points = Num1Points.data();
            pointCount = Num1Points.size();
            break;
        case 2:
            points = Num2Points.data();
            pointCount = Num2Points.size();
            break;
        case 3:
            points = Num3Points.data();
            pointCount = Num3Points.size();
            break;
        case 4:
            points = Num4Points.data();
            pointCount = Num4Points.size();
            break;
        case 5:
            points = Num5Points.data();
            pointCount = Num5Points.size();
            break;
        case 6:
            points = Num6Points.data();
            pointCount = Num6Points.size();
            break;
        case 7:
            points = Num7Points.data();
            pointCount = Num7Points.size();
            break;
        case 8:
            points = Num8Points.data();
            pointCount = Num8Points.size();
            break;
        case 9:
            points = Num9Points.data();
            pointCount = Num9Points.size();
            break;
        default:
            return QVariant();
    }

    // father forgive me for I have sinned
    static std::array<LinePoint, 100> linePoints{};
    for (size_t i = 0; i < pointCount; i++) {
        linePoints[i] = (LinePoint){
            static_cast<float>(center.x() + points[i].x() * NumberScale),
            static_cast<float>(center.y() + points[i].y() * -NumberScale),
            25, 25, 0, 255};
    }

    // connect last point to the start
    // TODO: Remove, once I have a proper font
    linePoints[pointCount] = (LinePoint){
        static_cast<float>(center.x() + points[0].x() * NumberScale),
        static_cast<float>(center.y() + points[0].y() * -NumberScale),
        0, 25, 0, 0};

    line.points = linePoints.data();
    line.pointCount = pointCount + 1;
    line.bounds.x = center.x() - CellRadius;
    line.bounds.y = center.y() - CellRadius;
    line.bounds.width = CellSize;
    line.bounds.height = CellSize;
    return QVariant::fromValue(line);
}
