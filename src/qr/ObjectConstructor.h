#ifndef __QR_OBJECT_CONSTRUCTOR_H__
#define __QR_OBJECT_CONSTRUCTOR_H__

#include "config.h"
#include <cassert>
#include <carve/poly.hpp>
#include "ViewBox.h"

namespace qr {
// -------------------------------------------------------------------------- //
// ObjectConstructor
// -------------------------------------------------------------------------- //
  class ObjectConstructor {
  public:
    ObjectConstructor(ViewBox* viewBox, int attempts): mViewBox(viewBox), mAttempts(attempts) {
      assert(attempts > 0);
    }

    carve::poly::Polyhedron* operator() ();

  private:
    ViewBox* mViewBox;
    int mAttempts;
  };

} // namespace qr

#endif // __QR_OBJECT_CONSTRUCTOR_H__
