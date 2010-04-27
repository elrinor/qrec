#ifndef __QR_SEGMENT_H__
#define __QR_SEGMENT_H__

#include "config.h"
#include "LinePrimitive.h"

namespace qr {
// -------------------------------------------------------------------------- //
// Segment
// -------------------------------------------------------------------------- //
  class Segment: public LinePrimitive {
  public:
    Segment(const Vector2d& start, const Vector2d& end, const QColor& color, const Qt::PenStyle style): LinePrimitive(start, end, color, style) {}

    virtual void draw(QPainter& painter) const;

    virtual Rect boundingRect() const;

  private:
  };

} // namespace qr

#endif // __QR_SEGMENT_H__
