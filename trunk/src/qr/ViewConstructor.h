#ifndef __QR_VIEW_CONSTRUCTOR_H__
#define __QR_VIEW_CONSTRUCTOR_H__

#include "config.h"
#include <QList>
#include "Drawing.h"
#include "View.h"

namespace qr {
// -------------------------------------------------------------------------- //
// ViewConstructor
// -------------------------------------------------------------------------- //
  class ViewConstructor {
  public:
    ViewConstructor(Drawing* drawing, double prec): mDrawing(drawing), mPrec(prec) {
      assert(drawing != NULL);
    }

    QList<View*> operator() ();

  private:
    View* closestView(const Vector2d& point) const;

    double mPrec;
    Drawing* mDrawing;
    QList<View*> mViews;
  };

} // namespace qr

#endif // __QR_VIEW_CONSTRUCTOR_H__
