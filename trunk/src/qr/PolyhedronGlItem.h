#ifndef __QR_POLYHEDRON_GL_ITEM_H__
#define __QR_POLYHEDRON_GL_ITEM_H__

#include "config.h"
#include <carve/poly.hpp>
#include "ViewGlItem.h"

namespace qr {
// -------------------------------------------------------------------------- //
// PolyhedronGlItem
// -------------------------------------------------------------------------- //
  class PolyhedronGlItem: public ViewGlItem {
  public:
    PolyhedronGlItem(carve::poly::Polyhedron* polyhedron);

    virtual void draw();

  private:
    carve::poly::Polyhedron* mPoly;
  };

} // namespace qr

#endif // __QR_POLYHEDRON_GL_ITEM_H__
