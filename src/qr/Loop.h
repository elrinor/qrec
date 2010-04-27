#ifndef __QR_LOOP_H__
#define __QR_LOOP_H__

#include "config.h"
#include <cassert>
#include <boost/noncopyable.hpp>
#include <QList>
#include "Primitive.h"
#include "Edge.h"

namespace qr {
// -------------------------------------------------------------------------- //
// Loop
// -------------------------------------------------------------------------- //
  class Loop: public Primitive, private boost::noncopyable {
  public:
    Loop(): mIsFundamental(false), mIsSolid(false), mIsDisjoint(false) {}

    void addEdge(Edge* edge) {
      assert(!mEdges.contains(edge));
      assert(mEdges.isEmpty() || mEdges.back()->isExtension(edge, 1.0e-6)); /* TODO: EPS */

      mEdges.push_back(edge);
    }

    const QList<Edge*>& edges() const {
      return mEdges;
    }

    bool isFundamental() const {
      return mIsFundamental;
    }

    void setFundamental(bool isFundamental) {
      mIsFundamental = isFundamental;
    }

    bool isSolid() const {
      return mIsSolid;
    }

    void setSolid(bool isSolid) {
      mIsSolid = isSolid;
    }

    bool isDisjoint() const {
      return mIsDisjoint;
    }

    void setDisjoint(bool isDisjoint) {
      mIsDisjoint = isDisjoint;
    }

  private:
    bool mIsSolid;
    bool mIsFundamental;
    bool mIsDisjoint;
    QList<Edge*> mEdges;
  };

} // namespace qr

#endif // __QR_LOOP_H__
