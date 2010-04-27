#ifndef __QR_G_SEGMENT_H__
#define __QR_G_SEGMENT_H__

#include "config.h"
#include <boost/array.hpp>
#include "Algebra.h"

namespace qr {
  template<class Vector>
  class GRect;

  template<class Vector>
  class GLine;


// -------------------------------------------------------------------------- //
// GSegment
// -------------------------------------------------------------------------- //
  template<class Vector>
  class GSegment {
  public:
    GSegment() {}

    GSegment(const Vector& end0, const Vector& end1) {
      assert(end0.size() == end1.size());
      mEnds[0] = end0;
      mEnds[1] = end1;
    }

    double length() const {
      return (mEnds[0] - mEnds[1]).norm();
    }

    template<int endIndex>
    const Vector& end() const {
      return mEnds[endIndex];
    }

    const Vector& end(int endIndex) const {
      return mEnds[endIndex];
    }

    const Vector& farthestEnd(const Vector& from) const {
      return mEnds[(from - mEnds[0]).squaredNorm() > (from - mEnds[1]).squaredNorm() ? 0 : 1];
    }

    const Vector& closestEnd(const Vector& to) const {
      return mEnds[(to - mEnds[0]).squaredNorm() < (to - mEnds[1]).squaredNorm() ? 0 : 1];
    }

    Vector center() const {
      return (mEnds[0] + mEnds[1]) / 2;
    }

    bool isParallel(const GSegment& other, double prec) const;
    
    bool isParallel(const GLine<Vector>& other, double prec) const;

    bool isCoincident(const GSegment& other, double prec) const;

    bool intersects(const GSegment& other, double prec) const;

    bool intersects(const GLine<Vector>& line, double prec) const;

    bool intersects(const GRect<Vector>& rect, double prec) const;

    GLine<Vector> asLine() const;

    GRect<Vector> boundingRect() const;

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW;

  protected:
    boost::array<Vector, 2> mEnds;
  };

  typedef GSegment<Vector2d> Segment2d;
  typedef GSegment<Vector3d> Segment3d;

} // namespace qr

#include "Interop.h"

#endif // __QR_G_SEGMENT_H__
