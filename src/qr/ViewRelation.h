#ifndef __QR_VIEW_RELATION_H__
#define __QR_VIEW_RELATION_H__

#include "config.h"
#include <boost/mpl/integral_c.hpp>
#include <boost/noncopyable.hpp>
#include <boost/array.hpp>
#include "Algebra.h"

namespace qr {
  class View;

// -------------------------------------------------------------------------- //
// ViewRelation
// -------------------------------------------------------------------------- //
  class ViewRelation: private boost::noncopyable {
  public:
    /* TODO: IMPROVEMENT: Parallel / Perpendicular with exact overlap => increased belief */
    enum Type {
      PARALLEL,
      PERPENDICULAR,
      NAME,
      CENTER
    };

    enum Direction {
      X,
      Y,
      MAX_DIRECTION = Y
    };

    static Direction directionOf(const Vector2d& vector) {
      return std::abs(vector.x()) > std::abs(vector.y()) ? X : Y;
    }

    static Direction perpendicularDirection(Direction direction) {
      return direction == X ? Y : X;
    }

    ViewRelation(Type type, View* source, View* target, Direction direction): mType(type), mDirection(direction), mSource(source), mTarget(target) {}

    View* source() const {
      return mSource;
    }

    View* target() const {
      return mTarget;
    }

    Type type() const {
      return mType;
    }

    Direction direction() const {
      return mDirection;
    }

    double belief() const {
      return mBelief;
    }

    void setBelief(double belief) {
      mBelief = belief;
    }

  private:
    View *mSource, *mTarget;
    Type mType;
    Direction mDirection;
    double mBelief;
  };

} // namespace qr

#endif // __QR_VIEW_RELATION_H__
