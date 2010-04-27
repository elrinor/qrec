#ifndef __QR_SEGMENT_CLASSIFIER_H__
#define __QR_SEGMENT_CLASSIFIER_H__

#include "config.h"
#include "Drawing.h"

namespace qr {
// -------------------------------------------------------------------------- //
// EdgeClassifier
// -------------------------------------------------------------------------- //
  class EdgeClassifier {
  public:
    EdgeClassifier(Drawing* drawing): mDrawing(drawing) {}

    void operator() ();

  private:
    Drawing* mDrawing;
  };

} // namespace qr

#endif // __QR_SEGMENT_CLASSIFIER_H__
