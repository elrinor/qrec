#ifndef __QR_PRIMITIVE_H__
#define __QR_PRIMITIVE_H__

#include "config.h"
#include <cassert>

namespace qr {
  class View;

// -------------------------------------------------------------------------- //
// Primitive
// -------------------------------------------------------------------------- //
  class Primitive {
  public:
    Primitive(): mView(NULL) {}

    View* view() const {
      return mView;
    }

    void setView(View* view) {
      assert(mView == NULL);

      mView = view;
    }
    
  private:
    View* mView;
  };

} // namespace qr

#endif // __QR_PRIMITIVE_H__
