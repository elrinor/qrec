#ifndef __QR_LOOP_MERGER_H__
#define __QR_LOOP_MERGER_H__

#include "config.h"
#include "Loop.h"

namespace qr {
// -------------------------------------------------------------------------- //
// LoopMerger
// -------------------------------------------------------------------------- //
  class LoopMerger {
  public:
    LoopMerger(Loop* first, Loop* second): mFirst(first), mSecond(second) {}
    
    Loop* operator() ();

  private:
    Loop *mFirst, *mSecond;
  };

} // namespace qr

#endif // __QR_LOOP_MERGER_H__
