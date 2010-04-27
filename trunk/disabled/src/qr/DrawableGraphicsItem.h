#ifndef __QR_DRAWABLE_GRAPHICS_ITEM_H__
#define __QR_DRAWABLE_GRAPHICS_ITEM_H__

#include "config.h"
#include <QGraphicsItem>
#include "Interop.h"
# if 0
namespace qr {
// -------------------------------------------------------------------------- //
// DrawingView
// -------------------------------------------------------------------------- //
  class DrawableGraphicsItem: public QGraphicsItem {
  public:
    DrawableGraphicsItem(Drawable* drawable): mDrawable(drawable) {}

    QRectF boundingRect() const {
      return toQRectF(mDrawable->boundingRect());
    }

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* /*option*/, QWidget* /*widget*/) {
      mDrawable->draw(*painter);
    }

  private:
    Drawable* mDrawable;
  };

} // namespace qr
#endif

#endif // __QR_DRAWABLE_GRAPHICS_ITEM_H__
