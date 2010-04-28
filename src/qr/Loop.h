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
    Loop(): mIsFundamental(false), mIsSolid(false), mIsDisjoint(false), mIsVerticesValid(false) {}

    void addEdge(Edge* edge) {
      assert(!mEdges.contains(edge));
      assert(mEdges.isEmpty() || mEdges.back()->isExtension(edge, 1.0e-6)); /* TODO: EPS */

      mEdges.push_back(edge);
      mIsVerticesValid = false;
    }

    const QList<Edge*>& edges() const {
      return mEdges;
    }

    Edge* edge(int index) const {
      return mEdges[index];
    }

    const QList<Vertex*>& vertices() const {
      assert(mEdges.size() > 1);

      if(!mIsVerticesValid) {
        mVertices.clear();
        for(int i0 = 0; i0 < mEdges.size(); i0++) {
          int i1 = (i0 + 1) % mEdges.size();

          Vertex* vertex = mEdges[i0]->commonVertex(mEdges[i1]);
          assert(vertex != NULL);
          mVertices.push_back(vertex);
        }
        mIsVerticesValid = true;
      }
      return mVertices;
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

    mutable bool mIsVerticesValid;
    mutable QList<Vertex*> mVertices;
  };

} // namespace qr

#endif // __QR_LOOP_H__
