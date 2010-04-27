#include "Arc.h"
#include <QPainter>

namespace qr {
  Arc::Arc(const Vector2d& center, const Vector2d& halfSize, double startAngle, double endAngle, const QColor& color, const Qt::PenStyle style): LinePrimitive(color, style), mCenter(center), mHalfSize(halfSize), mStartAngle(startAngle), mEndAngle(mEndAngle) {
    Vector2d right = Vector2d(halfSize.x(), 0.0);
    Vector2d up = Vector2d(0.0, halfSize.y());

    mStart = center + right * cos(startAngle) + up * sin(startAngle);
    mEnd   = center + right * cos(  endAngle) + up * sin(  endAngle);
  }

  void Arc::draw(QPainter& painter) const {
    painter.setPen(mPen);

    Vector2d topLeft = mCenter - mHalfSize;

    painter.drawArc(
      QRectF(topLeft.x(), topLeft.y(), mHalfSize.x() * 2, mHalfSize.y() * 2), 
      static_cast<int>(mStartAngle / M_PI * 180 * 16), 
      static_cast<int>((mEndAngle - mStartAngle) / M_PI * 180 * 16)
    );
  }

  Rect Arc::boundingRect() const { /* TODO */
    return Rect(mStart.cwise().min(mEnd), (mStart - mEnd).cwise().abs());
  }


}