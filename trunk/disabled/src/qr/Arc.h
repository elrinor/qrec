#ifndef __QR_ARC_H__
#define __QR_ARC_H__

#include "config.h"
#include "LinePrimitive.h"

namespace qr {
// -------------------------------------------------------------------------- //
// Arc
// -------------------------------------------------------------------------- //
  class Arc: public LinePrimitive {
  public:
    /**
     * @param center
     * @param halfSize
     * @param startAngle
     * @param endAngle                 In radians.
     */
    Arc(const Vector2d& center, const Vector2d& halfSize, double startAngle, double endAngle, const QColor& color, const Qt::PenStyle style);

    virtual void draw(QPainter& painter) const;

    virtual Rect boundingRect() const;

  private:
    Vector2d mCenter, mHalfSize;
    double mStartAngle, mEndAngle;
  };

} // namespace qr

#endif // __QR_ARC_H__
