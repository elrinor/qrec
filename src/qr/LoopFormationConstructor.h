#ifndef __QR_LOOP_FORMATION_CONSTRUCTOR_H__
#define __QR_LOOP_FORMATION_CONSTRUCTOR_H__

#include "config.h"
#include "ViewBox.h"

namespace qr {
// -------------------------------------------------------------------------- //
// LoopFormationConstructor
// -------------------------------------------------------------------------- //
  class LoopFormationConstructor {
  public:
    LoopFormationConstructor(ViewBox* viewBox, double prec): mViewBox(viewBox), mPrec(prec) {}

    void operator() ();

  private:
    ViewBox* mViewBox;
    double mPrec;
  };

} // namespace qr

#endif // __QR_LOOP_FORMATION_CONSTRUCTOR_H__
