#include "LoopFormationExtruder.h"
#include <algorithm> /* for std::random_shuffle() */
#include <iterator> /* for std::back_inserter() */
#include <boost/foreach.hpp>
#include <carve/csg.hpp>
#include <carve/tree.hpp>
#include <carve/input.hpp>
#include "LoopExtruder.h"
#include "Debug.h"
#include "View.h"

namespace qr {
  namespace { 
    bool isCircle(Loop* loop, double prec) {
      foreach(Edge* edge, loop->edges())
        if(edge->type() != Edge::ARC || std::abs(edge->asArc().longAxis().norm() - edge->asArc().shortAxis().norm()) > prec)
          return false;

      Vector2d center = loop->edge(0)->asArc().center();
      foreach(Edge* edge, loop->edges())
        if(!(edge->asArc().center() - center).isZero(prec))
          return false;
      return true;
    }

    bool isSphere(LoopFormation* loopFormation, double prec) {
      foreach(Loop* loop, loopFormation->loops())
        if(!isCircle(loop, prec))
          return false;
      return true;
    }

    Vector3d spherePoint(const Vector3d& center, double radius, double phi, double psi) {
      return center + radius * Vector3d(cos(phi) * cos(psi), sin(phi) * cos(psi), sin(psi));
    }

    carve::geom::vector<3> toVECTOR(const Vector3d& v) {
      return carve::geom::VECTOR(v.x(), v.y(), v.z());
    }

  } // namespace

// -------------------------------------------------------------------------- //
// LoopFormationExtruder
// -------------------------------------------------------------------------- //
  carve::poly::Polyhedron* LoopFormationExtruder::operator() () {
    carve::poly::Polyhedron* result = NULL;

    /* TODO: we better move this to LoopFormationConstructor and add Class enum to LoopFormation (i.e. NORMAL, SPHERE, etc...) */
    if(isSphere(mLoopFormation, 1.0e-6)) { /* TODO: EPS */
      Vector3d center;
      foreach(Loop* loop, mLoopFormation->loops()) {
        Vector2d center2d = loop->edge(0)->asArc().center();
        Vector3d center3d = loop->view()->transform() * Vector3d(center2d.x(), center2d.y(), 0.0);

        int idx0 = (loop->view()->perpendicularAxisIndex() + 1) % 3;
        int idx1 = (loop->view()->perpendicularAxisIndex() + 2) % 3;

        center[idx0] = center3d[idx0];
        center[idx1] = center3d[idx1];
      }

      double radius = mLoopFormation->loop(0)->edge(0)->asArc().longAxis().norm();

      carve::input::PolyhedronData data;

      const int STACKS = 16;
      const int SLICES = 32;

      for(int j = 0; j < STACKS - 1; j++)
        for(int i = 0; i < SLICES; i++)
          data.addVertex(toVECTOR(spherePoint(center, radius, i * 2 * M_PI / SLICES, (j + 1) * M_PI / STACKS - M_PI / 2)));
      data.addVertex(toVECTOR(spherePoint(center, radius, 0, -M_PI / 2)));
      data.addVertex(toVECTOR(spherePoint(center, radius, 0, M_PI / 2)));

      int n = data.points.size() - 2;

      for(int i = 0; i < SLICES; i++) {
        data.addFace(n, (i + 1) % SLICES, i);
        data.addFace(n + 1, n - 1 - (i + 1) % SLICES, n - 1 - i);
      }

      for(int j = 0; j < STACKS - 2; j++) {
        for(int i = 0; i < SLICES; i++) {
          int i1 = (i + 1) % SLICES;
          int j1 = j + 1;
          data.addFace(j * SLICES + i, j * SLICES + i1, j1 * SLICES + i1, j1 * SLICES + i);
        }
      }

      result = new carve::poly::Polyhedron(data.points, data.getFaceCount(), data.faceIndices);
    } else {
      QList<Loop*> loops;
      std::copy(mLoopFormation->loops().begin(), mLoopFormation->loops().end(), std::back_inserter(loops));

      /* Put all arcs in tail. */
      for(int i = 0; i < loops.size(); i++) {
        for(int j = i + 1; j < loops.size(); j++) {
          bool hasArcs = false;
          foreach(Edge* edge, loops[i]->edges())
            if(edge->type() == Edge::ARC)
              hasArcs = true;
          if(hasArcs)
            std::swap(loops[i], loops[j]);
        }
      }

      for(int i = 0; i < mAttempts; i++) {
        carve::csg::CSG_TreeNode* node = NULL;
        foreach(Loop* loop, loops) {
          carve::poly::Polyhedron* extrudedLoop = LoopExtruder(loop)();
          if(node == NULL)
            node = new carve::csg::CSG_PolyNode(extrudedLoop, true);
          else
            node = new carve::csg::CSG_OPNode(new carve::csg::CSG_PolyNode(extrudedLoop, true), node, carve::csg::CSG::INTERSECTION, true);
        }

        try {
          carve::csg::CSG csg;
          result = node->eval(csg);
          break;
        } catch (carve::exception&) {}

        std::random_shuffle(loops.begin(), loops.end());
      }
    }

    result->canonicalize();
    //debugShowPoly(result);
    return result;
  }

} // namespace qr
