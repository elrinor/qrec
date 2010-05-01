#ifndef __QR_LOOP_H__
#define __QR_LOOP_H__

#include "config.h"
#include <cassert>
#include <algorithm> /* for std::reverse() */
#include <boost/noncopyable.hpp>
#include <boost/foreach.hpp>
#include <QList>
#include <QSet>
#include "Primitive.h"
#include "Edge.h"
#include "Vertex.h"

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
    Loop(): 
      mIsFundamental(false), 
      mIsSolid(false),
      mIsDisjoint(false), 
      mIsHatched(false), 
      mIsVerticesValid(false),
      mIsVertexSetValid(false),
      mIsBoundingRectValid(false), 
      mIsBoundingRect3dValid(false)
    {}

    void addEdge(Edge* edge) {
      assert(!mEdges.contains(edge));
      assert(mEdges.isEmpty() || mEdges.back()->isExtension(edge, 1.0e-6)); /* TODO: EPS */

      mEdges.push_back(edge);
      mIsVerticesValid = false;
      mIsVertexSetValid = false;
      mIsBoundingRectValid = false;
      mIsBoundingRect3dValid = false;
    }

    const QList<Edge*>& edges() const {
      return mEdges;
    }

    Edge* edge(int index) const {
      return mEdges[index];
    }

    void reverse() {
      std::reverse(mEdges.begin(), mEdges.end());
      mIsVertexSetValid = false;
      mIsVerticesValid = false;
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

    const QSet<Vertex*>& vertexSet() const {
      if(!mIsVertexSetValid) {
        vertices();
        mVertexSet.clear();
        foreach(const LoopVertex& loopVertex, mVertices)
          mVertexSet.insert(loopVertex.vertex());
        mIsVertexSetValid = true;
      }
      return mVertexSet;
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

    bool isHatched() const {
      return mIsHatched;
    }

    void setHatched(bool isHatched) {
      mIsHatched = isHatched;
    }

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW;

    const Rect2d& boundingRect() const {
      if(!mIsBoundingRectValid) {
        vertices();
        mBoundingRect = Rect2d();
        foreach(const LoopVertex& loopVertex, mVertices)
          mBoundingRect.extend(loopVertex.vertex()->pos2d());
        mIsBoundingRectValid = true;
      }
      return mBoundingRect;
    }

    const Rect3d& boundingRect3d() const {
      if(!mIsBoundingRect3dValid) {
        vertices();
        mBoundingRect3d = Rect3d();
        foreach(const LoopVertex& loopVertex, mVertices)
          mBoundingRect3d.extend(loopVertex.vertex()->pos3d());
        mIsBoundingRect3dValid = true;
      }
      return mBoundingRect3d;
    }

  private:
    bool mIsSolid;
    bool mIsFundamental;
    bool mIsDisjoint;
    bool mIsHatched;
    QList<Edge*> mEdges;

    mutable bool mIsVerticesValid;
    mutable QList<LoopVertex> mVertices;

    mutable bool mIsVertexSetValid;
    mutable QSet<Vertex*> mVertexSet;

    mutable bool mIsBoundingRectValid;
    mutable Rect2d mBoundingRect;

    mutable bool mIsBoundingRect3dValid;
    mutable Rect3d mBoundingRect3d;
  };

} // namespace qr

#endif // __QR_LOOP_H__
