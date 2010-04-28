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
// LoopVertex
// -------------------------------------------------------------------------- //
  class LoopVertex {
  public:
    enum Type {
      NORMAL,
      TANGENT
    };

    LoopVertex(Type type, Edge* prevEdge, Edge* nextEdge, Vertex* vertex): mType(type), mPrevEdge(prevEdge), mNextEdge(nextEdge), mVertex(vertex) {
      assert(prevEdge->commonVertex(nextEdge) == vertex);
      assert(vertex != NULL);
    };

    Type type() const {
      return mType;
    }

    Vertex* vertex() const {
      return mVertex;
    }

    Edge* prevEdge() const {
      return mPrevEdge;
    }

    Edge* nextEdge() const {
      return mNextEdge;
    }

  private:
    Type mType;
    Vertex* mVertex;
    Edge* mPrevEdge;
    Edge* mNextEdge;
  };


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

    const QList<LoopVertex>& vertices() const {
      assert(mEdges.size() > 1);

      if(!mIsVerticesValid) {
        mVertices.clear();
        for(int i0 = 0; i0 < mEdges.size(); i0++) {
          Edge* prevEdge = mEdges[i0];
          Edge* nextEdge = mEdges[(i0 + 1) % mEdges.size()];
          Vertex* vertex = prevEdge->commonVertex(nextEdge);
          LoopVertex::Type type = isCollinear(prevEdge->tangent(vertex), nextEdge->tangent(vertex), 1.0e-4) ? LoopVertex::TANGENT : LoopVertex::NORMAL; /* TODO: EPS */
          mVertices.push_back(LoopVertex(type, prevEdge, nextEdge, vertex)); 
        }
        mIsVerticesValid = true;
      }
      return mVertices;
    }

    const LoopVertex& vertex(int index) const {
      if(!mIsVerticesValid)
        vertices();
      return mVertices[index];
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
    mutable QList<LoopVertex> mVertices;
  };

} // namespace qr

#endif // __QR_LOOP_H__
