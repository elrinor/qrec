#ifndef __QR_PRIMITIVE_GRAPHICS_ITEM_FACTORY_H__
#define __QR_PRIMITIVE_GRAPHICS_ITEM_FACTORY_H__

#include "config.h"
#include "PrimitiveGraphicsItem.h"
#include "HatchGraphicsItem.h"
#include "LabelGraphicsItem.h"
#include "SegmentGraphicsItem.h"

namespace qr {
// -------------------------------------------------------------------------- //
// PrimitiveGraphicsItemFactory
// -------------------------------------------------------------------------- //
  class PrimitiveGraphicsItemFactory {
  public:
    PrimitiveGraphicsItemFactory() {}

    HatchGraphicsItem* operator() (Hatch* hatch) const {
      return new HatchGraphicsItem(hatch);
    }

    LabelGraphicsItem* operator() (Label* label) const {
      return new LabelGraphicsItem(label);
    }

    SegmentGraphicsItem* operator() (Segment* segment) const {
      return new SegmentGraphicsItem(segment);
    }

  private:
  };

} // namespace qr

#endif // __QR_PRIMITIVE_GRAPHICS_ITEM_FACTORY_H__
