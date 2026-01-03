#include "rm_Line.hpp"

Line Line::fromPoints(QList<LinePoint> &&points, const QRectF& bounds) {
    Line line = {};
    line.tool = 0x13; // SolidPen
    line.color = 0; // Black
    line.rgba = 0xff000000;
    line.points = QList(points.begin(), points.end());
    line.maskScale = 2.0;
    line.thickness = 0.0f;
    line.bounds = bounds;
    return line;
}

Line Line::fromPoints(QList<LinePoint> &&points, const QPointF& center, float radius) {
    QRectF bounds = {
        center.x() - radius,
        center.y() - radius,
        radius * 2.0f,
        radius * 2.0f
    };
    return fromPoints(std::move(points), bounds);
}

Line Line::fromPoints(std::span<const LinePoint> _points, const QRectF& bounds) {
    auto points = QList<LinePoint>(_points.begin(), _points.end());
    return fromPoints(std::move(points), bounds);
}

Line Line::fromPoints(std::span<const LinePoint> points, const QPointF& center, float radius) {
    QRectF bounds = {
        center.x() - radius,
        center.y() - radius,
        radius * 2.0f,
        radius * 2.0f
    };
    return fromPoints(points, bounds);
}
