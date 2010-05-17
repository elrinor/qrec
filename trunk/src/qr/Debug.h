#ifndef __QR_DEBUG_H__
#define __QR_DEBUG_H__

#include "config.h"
#include <carve/poly.hpp>
#include <carve/csg.hpp>

namespace qr {
  void debugShowPoly(carve::poly::Polyhedron* poly);

  void debugSavePoly(carve::poly::Polyhedron* poly, carve::csg::CSG::OP op);

} // namespace qr

#endif // __QR_DEBUG_H__
