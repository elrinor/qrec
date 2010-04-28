#ifndef __QR_VIEW_BOX_GL_ITEM_H__
#define __QR_VIEW_BOX_GL_ITEM_H__

#include "config.h"
#include "ViewBox.h"
#include "ViewGlItem.h"

namespace qr {
// -------------------------------------------------------------------------- //
// ViewBoxGlItem
// -------------------------------------------------------------------------- //
  class ViewBoxGlItem: public ViewGlItem {
  public:
    ViewBoxGlItem(ViewBox* viewBox): mViewBox(viewBox) {}

    virtual void draw();

  private:
    ViewBox* mViewBox;
  };

} // namespace qr

#endif // __QR_VIEW_BOX_GL_ITEM_H__
