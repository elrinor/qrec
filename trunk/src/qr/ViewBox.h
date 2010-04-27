#ifndef __QR_VIEW_BOX_H__
#define __QR_VIEW_BOX_H__

#include "config.h"
#include <boost/array.hpp>
#include <boost/noncopyable.hpp>
#include <QList>
#include "GRect.h"
#include "View.h"

namespace qr {
// -------------------------------------------------------------------------- //
// ViewBox
// -------------------------------------------------------------------------- //
  class ViewBox: boost::noncopyable {
  public:
    ViewBox(): mIsBoundingRectValid(false) {}

    void addView(View* view) {
      assert(view->projectionPlane() != View::UNKNOWN);

      mViews[view->projectionPlane()].push_back(view);
      mAllViews.push_back(view);
      mIsBoundingRectValid = false;
    }

    const QList<View*>& views(View::ProjectionPlane projectionPlane) const {
      return mViews[projectionPlane];
    }

    const QList<View*>& views() const {
      return mAllViews;
    }

    const Rect3d& boundingRect() {
      if(!mIsBoundingRectValid) {
        mBoundingRect = Rect3d();

        foreach(View* view, mAllViews) {
          Rect2d rect = view->boundingRect();
          Vector3d v0 = view->transform() * Vector3d(rect.min(0), rect.min(1), 0.0);
          Vector3d v1 = view->transform() * Vector3d(rect.max(0), rect.min(1), 0.0);
          Vector3d v2 = view->transform() * Vector3d(rect.max(0), rect.max(1), 0.0);
          Vector3d v3 = view->transform() * Vector3d(rect.min(0), rect.max(1), 0.0);
          mBoundingRect.extend(v0);
          mBoundingRect.extend(v1);
          mBoundingRect.extend(v2);
          mBoundingRect.extend(v3);
        }
      }
      return mBoundingRect;
    }


  private:
    boost::array<QList<View*>, View::MAX_PROJECTION_PLANE + 1> mViews;
    QList<View*> mAllViews;

    mutable bool mIsBoundingRectValid;
    mutable Rect3d mBoundingRect;
  };
}


#endif // __QR_VIEW_BOX_H__
