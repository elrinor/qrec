#include "Hatch.h"
#include <limits>
#include <QPainter>

namespace qr {
  Rect Hatch::boundingRect() const {
    double minX, minY, maxX, maxY;
    minX = minY = std::numeric_limits<double>::max();
    maxX = maxY = std::numeric_limits<double>::min();
    
    foreach(const QList<Vector2d>& loop, mLoops) {
      foreach(const Vector2d& point, loop) {
        minX = std::min(minX, point.x());
        minY = std::min(minY, point.y());
        maxX = std::max(maxX, point.x());
        maxY = std::max(maxY, point.y());
      }
    }

    return Rect(minX, minY, maxX - minX, maxY - minY);
  }

  void Hatch::draw(QPainter& painter) const {
    /* TODO: we cheat here. */
    if(mLoops.size() <= 0)
      return;

    QPolygonF polygon;
    foreach(const Vector2d& point, mLoops[0])
      polygon.append(QPointF(point.x(), point.y()));

    painter.setBrush(mBrush);
    painter.drawPolygon(polygon);
  }

} // namespace qr
