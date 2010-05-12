#ifndef __QR_VERTEX_H__
#define __QR_VERTEX_H__

#include "config.h"
#include <cassert>
#include <boost/noncopyable.hpp>
#include <QList>
#include "Algebra.h"
#include "Primitive.h"
#include "Edge.h"

namespace qr {
// -------------------------------------------------------------------------- //
// Vertex
// -------------------------------------------------------------------------- //
  class Vertex: public Primitive, private boost::noncopyable {
  public:
    enum Type {
      NORMAL,
      VIRTUAL
    };

    Vertex(const Vector2d& pos2d): mPos2d(pos2d), mType(NORMAL) {}

    Type type() const {
      return mType;
    }

    void setType(Type type) {
      mType = type;
    }

    const Vector2d& pos2d() const {
      return mPos2d;
    }

    double pos2d(int index) const {
      return mPos2d[index];
    }

    const Vector3d& pos3d() const {
      return mPos3d;
    }

    double pos3d(int index) const {
      return mPos3d[index];
    }

    void setPos3d(const Vector3d& pos3d) {
      mPos3d = pos3d;
    }

    const QList<Edge*>& edges() const {
      return mEdges;
    }

    void addEdge(Edge* edge) {
      assert((edge->end(0) - mPos2d).isZero(1.0e-6) || (edge->end(1) - mPos2d).isZero(1.0e-6));  /* TODO: EPS */
      assert(!mEdges.contains(edge));

      mEdges.push_back(edge);
    }

    void removeEdge(Edge* edge) {
      assert(mEdges.contains(edge));

      mEdges.removeOne(edge);
    }

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW;

  private:
    Vector2d mPos2d;
    Vector3d mPos3d;
    QList<Edge*> mEdges;
    Type mType;
  };


  inline Rect3d Edge::boundingRect3d() const {
    Rect3d result;
    result.extend(vertex(0)->pos3d());
    result.extend(vertex(1)->pos3d());
    return result;
  }

} // namespace qr

#endif // __QR_VERTEX_H__
