#ifndef __QR_HATCH_H__
#define __QR_HATCH_H__

#include "config.h"
#include <algorithm> /* for std::find() */
#include <boost/noncopyable.hpp>
#include <boost/foreach.hpp>
#include <QBrush>
#include <QList>
#include "Primitive.h"
#include "Algebra.h"

namespace qr {
  class View;

// -------------------------------------------------------------------------- //
// Hatch
// -------------------------------------------------------------------------- //
  class Hatch: public Primitive, private boost::noncopyable {
  public:
    Hatch(QBrush& brush): mBrush(brush), mIsBoundingRectValid(false) {}

    const QList<Edge*>& segments() const {
      return mSegments;
    }

    QList<Edge*>& segments() {
      return mSegments;
    }

    Edge* const& segment(int index) const {
      return mSegments[index];
    }

    const QBrush& brush() const {
      return mBrush;
    }

    void addSegment(Edge* segment) {
      mSegments.push_back(segment);
      mIsBoundingRectValid = false;
    }

    void replaceSegment(Edge* hatchSegment, Edge* newSegment) {
      /* TODO: speed this up. */
      QList<Edge*>::iterator pos = std::find(mSegments.begin(), mSegments.end(), hatchSegment);
      assert(pos != mSegments.end());
      *pos = newSegment;
    }

    Rect2d boundingRect() const {
      if(!mIsBoundingRectValid) {
        mBoundingRect = Rect2d();
        foreach(Edge* segment, mSegments)
          mBoundingRect.extend(segment->boundingRect());
        mIsBoundingRectValid = true;
      }
      return mBoundingRect;
    }

    /* We have GRect as a field, so we need to use aligned new... */
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW;

  private:
    QBrush mBrush;
    QList<Edge*> mSegments;
    mutable bool mIsBoundingRectValid;
    mutable Rect2d mBoundingRect;
  };

} // namespace qr 

#endif // __QR_HATCH_H__
