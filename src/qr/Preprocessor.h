#ifndef __QR_SEGMENT_PREPROCESSOR_H__
#define __QR_SEGMENT_PREPROCESSOR_H__

#include "config.h"
#include "Drawing.h"

namespace qr {
// -------------------------------------------------------------------------- //
// Preprocessor
// -------------------------------------------------------------------------- //
  class Preprocessor {
  public:
    Preprocessor(Drawing* drawing, double prec): mDrawing(drawing), mPrec(prec) {}

    void operator() ();

  private:
    Drawing* mDrawing;
    double mPrec;
  };

} // namespace qr

#endif // __QR_SEGMENT_PREPROCESSOR_H__
