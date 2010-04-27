#ifndef __QR_G_LINE_H__
#define __QR_G_LINE_H__

#include "config.h"
#include "Algebra.h"

namespace qr {
  template<class Vector>
  class GSegment;

  template<class Vector>
  class GRect;

// -------------------------------------------------------------------------- //
// GLine
// -------------------------------------------------------------------------- //
  /**
   * GLine defines a 1-dimensional linear manifold in N-dimensional space.
   */
  template<class Vector>
  class GLine {
  public:
    GLine() {}

    /**
     * Constructs line L(t) = at + direction
     */
    GLine(const Vector& point0, const Vector& point1): mOrigin(point0), mDirection(point1 - point0) {}

    const Vector& origin() const {
      return mOrigin;
    }

    const Vector& direction() const {
      return mDirection;
    }

    bool isParallel(const GLine& other, double prec) const;

    bool isParallel(const GSegment<Vector>& segment, double prec) const;

    bool isCoincident(const GLine& other, double prec) const;

    bool intersects(const GLine& other, double prec) const;
    
    bool intersects(const GSegment<Vector>& segment, double prec) const;

    bool intersects(const GRect<Vector>& rect, double prec) const;

    bool contains(const Vector& point, double prec) const;

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW;

  private:
    Vector2d mOrigin, mDirection;
  };

  typedef GLine<Vector2d> Line2d;
  typedef GLine<Vector3d> Line3d;

} // namespace qr

#include "Interop.h"

#endif // __QR_G_LINE_H__
