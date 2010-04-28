#ifndef __QR_LOOP_FORMATION_EXTRUDER_H__
#define __QR_LOOP_FORMATION_EXTRUDER_H__

#include "config.h"
#include <cassert>
#include <QList>
#include <carve/poly.hpp>
#include "Loop.h"

namespace qr {
// -------------------------------------------------------------------------- //
// LoopFormationExtruder
// -------------------------------------------------------------------------- //
  class LoopFormationExtruder {
  public:
    template<class LoopRange>
    LoopFormationExtruder(const LoopRange& loops, int attempts): mAttempts(attempts) {
      assert(attempts > 0);

      foreach(Loop* loop, loops)
        mLoops.push_back(loop);
    }

    carve::poly::Polyhedron* operator() ();

  private:
    QList<Loop*> mLoops;
    int mAttempts;
  };

} // namespace qr


#endif // __QR_LOOP_FORMATION_EXTRUDER_H__
