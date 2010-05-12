#ifndef __QR_INTEROP_H__
#define __QR_INTEROP_H__

#include "Algebra.h"
#include <algorithm> /* for std::swap() */
#include <QRectF>
#include <QPointF>
#include "GRect.h"
#include "GLine.h"
#include "GSegment.h"

namespace qr {
// -------------------------------------------------------------------------- //
// Conversions
// -------------------------------------------------------------------------- //
  inline QRectF toQRectF(const Rect2d& rect) {
    return QRectF(rect.min(0), rect.min(1), rect.size(0), rect.size(1));
  }

  inline QPointF toQPointF(const Vector2d& point) {
    return QPointF(point.x(), point.y());
  }


  namespace detail {
// -------------------------------------------------------------------------- //
// Intersection details
// -------------------------------------------------------------------------- //
    template<class VectorA0, class VectorB0, class VectorA1, class VectorB1>
    inline bool intersect(const VectorA0& a0, const VectorB0& b0, const VectorA1& a1, const VectorB1& b1, Vector2d* t) {
      assert(a0.size() == b0.size() && a1.size() == b1.size() && a0.size() == a1.size());

      Matrix<double, VectorA0::RowsAtCompileTime, 2> m(a0.size(), 2);
      m.col(0) = b0;
      m.col(1) = -b1;

      return m.lu().solve(a1 - a0, t);
    }

    template<class Matrix>
    inline bool linearlyDependent(const Matrix& m, double prec) {
      if(m.rows() < m.cols())
        return true;

      Eigen::LU<Matrix> lu = m.lu();
      return std::abs(lu.matrixLU()(m.cols() - 1, m.cols() - 1)) < prec;
    }

    template<class Vector0, class Vector1>
    inline bool linearlyDependent(const Vector0& v0, const Vector1& v1, double prec) {
      assert(v0.size() == v1.size());

      if(v0.size() < 2)
        return true;

      Matrix<double, Vector0::RowsAtCompileTime, 2> m(v0.size(), 2);
      m.col(0) = v0;
      m.col(1) = v1;
      return linearlyDependent(m, prec);
    }

    inline Vector1d vector1d(double point) {
      Vector1d result;
      result[0] = point;
      return result;
    }

    inline Rect1d rect1d(double point0, double point1) {
      if(point0 > point1)
        std::swap(point0, point1);
      return Rect1d(vector1d(point0), vector1d(point1 - point0));
    }

    template<class VectorOrigin, class VectorDirection>
    inline void clip(Rect1d& segment, const VectorOrigin& origin, const VectorDirection& direction, int axis, const Rect1d& axisSegment, double prec) {
      assert(origin.size() == direction.size());

      double originValue = origin[axis];
      double directionValue = direction[axis];

      if(std::abs(directionValue) < prec) {
        if(!axisSegment.contains(vector1d(originValue), prec))
          segment = Rect1d::emptyRect();
        return;
      }

      double min = (axisSegment.min(0) - originValue) / directionValue;
      double max = (axisSegment.max(0) - originValue) / directionValue;
      if(min > max)
        std::swap(min, max);

      segment.intersect(rect1d(min, max));
    }

    template<class Vector, class VectorOrigin, class VectorDirection>
    inline bool intersects(const GRect<Vector>& rect, const VectorOrigin& origin, const VectorDirection& direction, Rect1d& intersection, double prec) {
      assert(origin.size() == direction.size() && rect.min().size() == origin.size());

      for(int i = 0; i < origin.size(); i++) {
        clip(intersection, origin, direction, i, rect1d(rect.min(i), rect.max(i)), prec);
        if(intersection.isEmpty(prec))
          return false;
      }
      return true;
    }

  } // namespace detail


// -------------------------------------------------------------------------- //
// Intersection tests
// -------------------------------------------------------------------------- //
  template<class Vector>
  inline bool GLine<Vector>::isParallel(const GLine<Vector>& other, double prec) const {
    /* Some notes on black magic involved.
     * Lines are parallel when their direction vectors are linearly dependent. */
    return detail::linearlyDependent(mDirection, other.mDirection, prec);
  }

  template<class Vector>
  inline bool GLine<Vector>::isParallel(const GSegment<Vector>& segment, double prec) const {
    return isParallel(segment.asLine());
  }

  template<class Vector>
  inline bool GLine<Vector>::isCoincident(const GLine<Vector>& other, double prec) const {
    if(!isParallel(other, prec))
      return false;

    /* Here the idea is the same as in isParallel().
     * Lines coincide <=> they are parallel and the vector connecting 
     * their origins is linearly dependent on the direction vector. */
    return detail::linearlyDependent(mDirection, mOrigin - other.mOrigin, prec);
  }

  /*template<class Vector>
  inline bool GLine<Vector>:intersects(const GLine<Vector>& other, double prec) const {
    Vector2d t;
    return detail::intersect(mOrigin, mDirection, other.mOrigin, other.mDirection, &t);
  }*/

  template<class Vector>
  inline bool GLine<Vector>::intersects(const GSegment<Vector>& segment, double prec) const {
    return segment.intersects(*this, prec);
  }

  template<class Vector>
  inline bool GLine<Vector>::intersects(const GRect<Vector>& rect, double prec) const {
    return rect.intersects(*this, prec);
  }

  template<class Vector>
  inline bool GLine<Vector>::contains(const Vector& point, double prec) const {
    return detail::linearlyDependent(mDirection, point - mOrigin, prec);
  }

  template<class Vector>
  inline bool GSegment<Vector>::isParallel(const GSegment<Vector>& other, double prec) const {
    return asLine().isParallel(other.asLine(), prec);
  }

  template<class Vector>
  inline bool GSegment<Vector>::isParallel(const GLine<Vector>& line, double prec) const {
    return asLine().isParallel(line, prec);
  }

  template<class Vector>
  inline bool GSegment<Vector>::isCoincident(const GSegment<Vector>& other, double prec) const {
    return
      ((mEnds[0] - other.mEnds[0]).isZero(prec) && (mEnds[1] - other.mEnds[1]).isZero(prec)) ||
      ((mEnds[0] - other.mEnds[1]).isZero(prec) && (mEnds[1] - other.mEnds[0]).isZero(prec));
  }

  template<class Vector>
  inline bool GSegment<Vector>::intersects(const GSegment<Vector>& other, double prec) const {
    if(asLine().isCoincident(other.asLine(), prec)) {
      Vector b = mEnds[1] - mEnds[0];
      return detail::rect1d(
        (other.mEnds[0] - mEnds[0]).dot(b) / b.squaredNorm(), 
        (other.mEnds[1] - mEnds[0]).dot(b) / b.squaredNorm()
      ).intersects(detail::rect1d(0.0, 1.0), prec);
    }

    Vector2d t;
    if(!detail::intersect(mEnds[0], mEnds[1] - mEnds[0], other.mEnds[0], other.mEnds[1] - other.mEnds[0], &t))
      return false;
    return (t.cwise() >= -prec).all() && (t.cwise() <= 1 + prec).all();
  }

  template<class Vector>
  inline bool GSegment<Vector>::intersects(const GLine<Vector>& line, double prec) const {
    if(asLine().isCoincident(line, prec))
      return true;

    Vector2d t;
    if(!detail::intersect(mEnds[0], mEnds[1] - mEnds[0], line.origin(), line.direction(), &t))
      return false;
    return t[0] >= -prec && t[0] <= 1 + prec;
  }

  template<class Vector>
  inline bool GSegment<Vector>::intersects(const GRect<Vector>& rect, double prec) const {
    return rect.intersects(*this, prec);
  }

  template<class Vector>
  inline bool GRect<Vector>::intersects(const GLine<Vector>& line, double prec) const {
    Rect1d intersection = Rect1d::infiniteRect();
    return detail::intersects(*this, line.origin(), line.direction(), intersection, prec);
  }

  template<class Vector>
  inline bool GRect<Vector>::intersects(const GSegment<Vector>& segment, double prec) const {
    Rect1d intersection = detail::rect1d(0, 1);
    return detail::intersects(*this, segment.end(0), segment.end(1) - segment.end(0), intersection, prec);
  }

  template<class Vector>
  inline bool GRect<Vector>::contains(const GSegment<Vector>& segment, double prec) const {
    return contains(segment.end(0), prec) && contains(segment.end(1), prec);
  }


// -------------------------------------------------------------------------- //
// Operations on vectors
// -------------------------------------------------------------------------- //
  template<class Vector>
  inline bool isCollinear(const Vector& v0, const Vector& v1, double prec) {
    return detail::linearlyDependent(v0, v1, prec);
  }


// -------------------------------------------------------------------------- //
// Conversions
// -------------------------------------------------------------------------- //
  template<class Vector>
  inline GLine<Vector> GSegment<Vector>::asLine() const {
    return GLine<Vector>(mEnds[0], mEnds[1]);
  }

  template<class Vector>
  inline GRect<Vector> GSegment<Vector>::boundingRect() const {
    Vector min = mEnds[0].cwise().min(mEnds[1]);
    Vector max = mEnds[0].cwise().max(mEnds[1]);
    return GRect<Vector>(min, max - min);
  }

} // namespace qr

#endif // __QR_INTEROP_H__
