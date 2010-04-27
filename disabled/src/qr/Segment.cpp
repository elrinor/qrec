#include "Segment.h"
#include <QPainter>

namespace qr {
  void Segment::draw(QPainter& painter) const {
    painter.setPen(mPen);
    painter.drawLine(QPointF(mStart.x(), mStart.y()), QPointF(mEnd.x(), mEnd.y()));
  }

  Rect Segment::boundingRect() const {
    return Rect(mStart.cwise().min(mEnd), (mStart - mEnd).cwise().abs());
  }


} // namespace qr
