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
    struct Cone {
      Vector3d base;
      Vector3d baseX, baseY;
      Vector3d height;
      Vector3d topX, topY;
    };

    enum Type {
      PROTRUSION,
      DEPRESSION,
      UNKNOWN = -1
    };

    enum Class {
      NORMAL,
      CONE
    };

    LoopFormation(): mType(UNKNOWN), mClass(NORMAL) {}

    void addLoop(Loop* loop) {
      assert(!mLoops.contains(loop));

      mLoops.push_back(loop);
    }
 
    const QList<Loop*> loops() const {
      return mLoops;
    }

    Loop* loop(int index) const {
      return mLoops[index];
    }

    Type type() const {
      return mType;
    }

    void setType(Type type) {
      mType = type;
    }

    Class clazz() const {
      return mClass;
    }

    void setClass(Class clazz) {
      mClass = clazz;
    }

    const Cone& asCone() const {
      assert(mClass == CONE);
      return mCone;
    }

    Cone& asCone() {
      assert(mClass == CONE);
      return mCone;
    }

  private:
    Type mType;
    Class mClass;
    QList<Loop*> mLoops;
    Cone mCone;
  };

} // namespace qr

#endif // __QR_LOOP_FORMATION_H__
