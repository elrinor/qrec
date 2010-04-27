#ifndef __PRIMITIVE_GRAPHICS_ITEM_H__
#define __PRIMITIVE_GRAPHICS_ITEM_H__

#include "config.h"
#include <QGraphicsItem>
#include "Primitive.h"

namespace qr {
// -------------------------------------------------------------------------- //
// PrimitiveGraphicsItem
// -------------------------------------------------------------------------- //
  class PrimitiveGraphicsItem: public QGraphicsItem {
  public:
    PrimitiveGraphicsItem(const Primitive* primitive): mPrimitive(primitive) {}

    /* TODO: general handling? */

  private:
    const Primitive* mPrimitive;
  };

} // namespace qr

#endif // __PRIMITIVE_GRAPHICS_ITEM_H__
