#ifndef __QR_VIEW_GL_ITEM_H__
#define __QR_VIEW_GL_ITEM_H__

#include "config.h"
#include "ViewBox.h"

namespace qr {
// -------------------------------------------------------------------------- //
// ViewBoxGlItem
// -------------------------------------------------------------------------- //
  class ViewBoxGlItem {
  public:
    ViewBoxGlItem(ViewBox* viewBox): mViewBox(viewBox) {}

    void draw() const;

  private:
    ViewBox* mViewBox;
  };

} // namespace qr

#endif // __QR_VIEW_GL_ITEM_H__
