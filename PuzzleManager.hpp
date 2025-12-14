#include <QObject>
#include <QPointF>
#include <QVariant>
#include <Sudoku.hpp>
#include <QList>

struct LinePoint;

struct LinePoint {
    float x;
    float y;
    unsigned short speed;
    unsigned short width;
    unsigned char direction;
    unsigned char pressure;
} __attribute__((packed));
static_assert(sizeof(LinePoint) == 0xe, "LinePoint size mismatch");

struct QRectF {
    double x;
    double y;
    double width;
    double height;
};

struct Line {
    int tool;
    int color;
    unsigned int rgba;

    // Technically a QList<LinePoint>
    // unsigned int pointRc;
    // const LinePoint* points;
    // unsigned int pointCount;
    QList<LinePoint> points;

    double maskScale;
    float thickness;
    // probably padding and not necessary
    unsigned int unk_x24;
    // these bounds are used for early hit testing on erase & select
    QRectF bounds;
};
static_assert(sizeof(Line) == 72);

class PuzzleManager : public QObject
{
    Q_OBJECT
public:
    explicit PuzzleManager(QObject *parent = nullptr) : QObject(parent) {}

    Q_INVOKABLE void line(const Line &line);

    Q_INVOKABLE Line createGrid();

    Q_INVOKABLE Line createCircle();

    Q_INVOKABLE Line createLine(const QPointF& start, const QPointF& end);

    Q_INVOKABLE QVariant getSudoku(int level);
    Q_INVOKABLE QVariant getNumber(
        const Sudoku& sudoku,
        int column, int row);
};
