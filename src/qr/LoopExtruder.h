#ifndef __QR_LOOP_EXTRUDER_H__
#define __QR_LOOP_EXTRUDER_H__

#include "config.h"
#include <carve/poly.hpp>
#include "Loop.h"

namespace qr {
// -------------------------------------------------------------------------- //
// LoopExtruder
// -------------------------------------------------------------------------- //
  class LoopExtruder {
  public:
    LoopExtruder(Loop* loop): mLoop(loop) {}

    carve::poly::Polyhedron* operator() ();
  private:
    Loop* mLoop;
  };

} // namespace qr

#endif // __QR_LOOP_EXTRUDER_H__
