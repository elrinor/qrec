#ifndef __QR_LOOP_FORMATION_H__
#define __QR_LOOP_FORMATION_H__

#include "config.h"
#include <cassert>
#include <boost/noncopyable.hpp>
#include <QList>
#include "Loop.h"

namespace qr {
// -------------------------------------------------------------------------- //
// LoopFormation
// -------------------------------------------------------------------------- //
  class LoopFormation: boost::noncopyable {
  public:
    enum Type {
      PROTRUSION,
      DEPRESSION,
      UNKNOWN = -1
    };

    LoopFormation(): mType(UNKNOWN) {}

    void addLoop(Loop* loop) {
      assert(!mLoops.contains(loop));

      mLoops.push_back(loop);
    }
 
    const QList<Loop*> loops() const {
      return mLoops;
    }

    Type type() const {
      return mType;
    }

    void setType(Type type) {
      mType = type;
    }

  private:
    Type mType;
    QList<Loop*> mLoops;
  };

} // namespace qr

#endif // __QR_LOOP_FORMATION_H__
