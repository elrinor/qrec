#ifndef __QR_LOOP_CONSTRUCTOR_H__
#define __QR_LOOP_CONSTRUCTOR_H__

#include "config.h"
#include <QList>
#include "View.h"

namespace qr {
// -------------------------------------------------------------------------- //
// LoopConstructor
// -------------------------------------------------------------------------- //
  class LoopConstructor {
  public:
    LoopConstructor(QList<View*> views, double prec): mViews(views), mPrec(prec) {}

    void operator() ();

  private:
    QList<View*> mViews;
    double mPrec;
  };

} // namespace qr

#endif // __QR_LOOP_CONSTRUCTOR_H__
