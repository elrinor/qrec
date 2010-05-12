#ifndef __QR_LOOP_UTILS_H__
#define __QR_LOOP_UTILS_H__

#include "config.h"
#include "Loop.h"
#include "LoopFormation.h"

namespace qr {
// -------------------------------------------------------------------------- //
// LoopUtils
// -------------------------------------------------------------------------- //
  class LoopUtils {
  public: 
    static bool isCircle(Loop* loop, double prec) {
      foreach(Edge* edge, loop->edges())
        if(edge->type() != Edge::ARC || std::abs(edge->asArc().longAxis().norm() - edge->asArc().shortAxis().norm()) > prec)
          return false;

      Vector2d center = loop->edge(0)->asArc().center();
      foreach(Edge* edge, loop->edges())
        if(!(edge->asArc().center() - center).isZero(prec))
          return false;
      return true;
    }

    static bool isSphere(LoopFormation* loopFormation, double prec) {
      foreach(Loop* loop, loopFormation->loops())
        if(!isCircle(loop, prec))
          return false;
      return true;
    }

    static bool isTrapezoid(Loop* loop, double prec) {
      if(loop->edges().size() != 4)
        return false;

      foreach(Edge* edge, loop->edges())
        if(edge->type() != Edge::LINE)
          return false;

      return isTrapezoid(loop->edge(0), loop->edge(1), loop->edge(2), loop->edge(3), prec) || isTrapezoid(loop->edge(1), loop->edge(2), loop->edge(3), loop->edge(0), prec);
    }

  private: 
    static bool isTrapezoid(Edge* l, Edge* u, Edge* r, Edge* d, double prec) {
      return u->asSegment().isParallel(d->asSegment(), prec);
    }

  };

} // namespace qr

#endif // __QR_LOOP_UTILS_H__
