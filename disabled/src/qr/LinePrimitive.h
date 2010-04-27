#ifndef __QR_LINE_PRIMITIVE_H__
#define __QR_LINE_PRIMITIVE_H__

#include "config.h"
#include <QPen>
#include "Algebra.h"
#include "Drawable.h"

namespace qr {
// -------------------------------------------------------------------------- //
// Primitive
// -------------------------------------------------------------------------- //
  class LinePrimitive: public Drawable {
  public:
    enum Type {
      Line,
      Arc
    };

    Vector2d start() const {
      return mStart;
    }

    Vector2d end() const {
      return mEnd;
    }

    QColor color() const {
      return mPen.color();
    }

    Qt::PenStyle style() const {
      return mPen.style();
    }

    const QPen& pen() const {
      return mPen;
    }
  
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW;

  protected:
    LinePrimitive(const QColor& color, const Qt::PenStyle style): mPen(QPen(QBrush(color), 0, style)) {}

    LinePrimitive(const Vector2d& start, const Vector2d& end, const QPen& pen): mStart(start), mEnd(end), mPen(pen) {}

    LinePrimitive(const Vector2d& start, const Vector2d& end, const QColor& color, const Qt::PenStyle style): mStart(start), mEnd(end), mPen(QPen(QBrush(color), 0, style)) {}

    LinePrimitive() {}

    class SegmentData

    Vector2d mStart, mEnd;
    QPen mPen;
    Type mType;

  };

} // namespace qr

#endif // __QR_LINE_PRIMITIVE_H__
