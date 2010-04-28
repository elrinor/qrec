#include "LoopExtruder.h"
#include <algorithm> /* for std::swap() */
#include <carve/input.hpp>
#include "ViewBox.h"
#include "Utility.h"

namespace qr {
// -------------------------------------------------------------------------- //
// LoopExtruder
// -------------------------------------------------------------------------- //
  carve::poly::Polyhedron* LoopExtruder::operator() () {
    carve::input::PolyhedronData data;

    int idx = mLoop->view()->perpendicularAxisIndex();
    double lo = mLoop->view()->viewBox()->boundingRect().min(idx);
    double hi = mLoop->view()->viewBox()->boundingRect().max(idx);

    if(std::abs(mLoop->edge(0)->vertex(0)->pos3d()[idx] - hi) < 1.0e-5) /* TODO: EPS */
      std::swap(lo, hi);

    /* Add vertices. */
    foreach(const LoopVertex& loopVertex, mLoop->vertices()) {
      if(loopVertex.prevEdge()->type() == Edge::LINE && loopVertex.nextEdge()->type() == Edge::LINE && loopVertex.type() == LoopVertex::TANGENT)
        continue;

      Edge* edge = loopVertex.nextEdge();

      if(edge->type() == Edge::LINE) {
        Vector3d v = loopVertex.vertex()->pos3d();

        v[idx] = lo;
        data.addVertex(carve::geom::VECTOR(v.x(), v.y(), v.z()));
        v[idx] = hi;
        data.addVertex(carve::geom::VECTOR(v.x(), v.y(), v.z()));
      } else if(edge->type() == Edge::ARC) {
        bool forward = edge->vertex(0) == loopVertex.vertex();

        int start = forward ? 0 : 10;
        int delta = forward ? 1 : -1;

        const Edge::ArcData& arc = edge->asArc();
        for(int i = 0; i < 10; i++) {
          Vector2d v2 = arc.point((start + i * delta) / 10.0);
          Vector3d v = mLoop->view()->transform() * Vector3d(v2.x(), v2.y(), 0.0);

          v[idx] = lo;
          data.addVertex(carve::geom::VECTOR(v.x(), v.y(), v.z()));
          v[idx] = hi;
          data.addVertex(carve::geom::VECTOR(v.x(), v.y(), v.z()));
        }
      } else {
        Unreachable();
      }
    }

    int n = data.points.size() / 2;

    /* Add side faces. */
    for(int i = 0; i < n; i++)
      data.addFace(i * 2 + 1, i * 2, (i * 2 + 2) % (n * 2), (i * 2 + 3) % (n * 2));

    /* Add basement faces. */
    std::vector<int> base;
    base.resize(n);
    for(int i = 0; i < n; i++)
      base[i] = 2 * (n - 1) - i * 2;
    data.addFace(base.begin(), base.end());
    for(int i = 0; i < n; i++)
      base[i] = i * 2 + 1;
    data.addFace(base.begin(), base.end());

    return new carve::poly::Polyhedron(data.points, data.getFaceCount(), data.faceIndices);
  }

} // namespace qr
