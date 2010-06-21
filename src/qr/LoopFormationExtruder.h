#ifndef __QR_LOOP_FORMATION_EXTRUDER_H__
#define __QR_LOOP_FORMATION_EXTRUDER_H__

#include "config.h"
#include <cassert>
#include <QList>
#include <carve/poly.hpp>
#include "LoopFormation.h"

namespace qr {
// -------------------------------------------------------------------------- //
// LoopFormationExtruder
// -------------------------------------------------------------------------- //
  class LoopFormationExtruder {
  public:
    LoopFormationExtruder(LoopFormation* loopFormation, int attempts, QList<carve::poly::Polyhedron*>& subtractions, QList<carve::poly::Polyhedron*>& additions): 
      mLoopFormation(loopFormation), mAttempts(attempts), mSubtractions(subtractions), mAdditions(additions)
    {
      assert(attempts > 0);
    }

    carve::poly::Polyhedron* operator() ();

  private:
    LoopFormation* mLoopFormation;
    int mAttempts;
    QList<carve::poly::Polyhedron*> &mSubtractions, &mAdditions;
  };

} // namespace qr


#endif // __QR_LOOP_FORMATION_EXTRUDER_H__
