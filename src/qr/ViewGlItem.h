#ifndef __QR_VIEW_GL_ITEM_H__
#define __QR_VIEW_GL_ITEM_H__

#include "config.h"

namespace qr {
// -------------------------------------------------------------------------- //
// ViewGlItem
// -------------------------------------------------------------------------- //
  class ViewGlItem {
  public:
    virtual void draw() = 0; 
  };

} // namespace qr

#endif // __QR_VIEW_GL_ITEM_H__