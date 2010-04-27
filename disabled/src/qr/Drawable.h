#ifndef __QR_DRAWABLE_H__
#define __QR_DRAWABLE_H__

#include "config.h"
#include "Rect.h"

class QPainter;

namespace qr {
// -------------------------------------------------------------------------- //
// Drawable
// -------------------------------------------------------------------------- //
  /**
   * Interface for everything that can be drawn.
   */
  class Drawable {
  public:
    virtual void draw(QPainter&) const = 0;

    virtual Rect boundingRect() const = 0;
  };

} // namespace qr

#endif // __QR_DRAWABLE_H__
