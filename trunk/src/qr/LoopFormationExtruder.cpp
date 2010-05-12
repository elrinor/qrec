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
#include "LoopUtils.h"

namespace qr {
  namespace { 
    Vector3d spherePoint(const Vector3d& center, double radius, double phi, double psi) {
      return center + radius * Vector3d(cos(phi) * cos(psi), sin(phi) * cos(psi), sin(psi));
    }

    carve::geom::vector<3> toVECTOR(const Vector3d& v) {
      return carve::geom::VECTOR(v.x(), v.y(), v.z());
    }

    carve::poly::Polyhedron* genCone(const Vector3d& base, const Vector3d& height, const Vector3d& baseX, const Vector3d& baseY, const Vector3d& topX, const Vector3d& topY) {
      if(baseX.cross(baseY).dot(height) < 0)
        return genCone(base + height, -height, topX, topY, baseX, baseY);

      carve::input::PolyhedronData data;

      const int SLICES = 32;

      for(int i = 0; i < SLICES; i++) {
        float a = i * 2 * M_PI / SLICES;
        data.addVertex(toVECTOR(base +          baseX * cos(a) + baseY * sin(a)));
        data.addVertex(toVECTOR(base + height + topX * cos(a) + topY * sin(a)));
      }

      /* Add side faces. */
      for(int i = 0; i < SLICES; i++)
        data.addFace(i * 2 + 1, i * 2, (i * 2 + 2) % (SLICES * 2), (i * 2 + 3) % (SLICES * 2));

      /* Add basement faces. */
      std::vector<int> face;
      face.resize(SLICES);
      for(int i = 0; i < SLICES; i++)
        face[i] = 2 * (SLICES - 1) - i * 2;
      data.addFace(face.begin(), face.end());
      for(int i = 0; i < SLICES; i++)
        face[i] = i * 2 + 1;
      data.addFace(face.begin(), face.end());

      carve::poly::Polyhedron* result = new carve::poly::Polyhedron(data.points, data.getFaceCount(), data.faceIndices);
      result->canonicalize();
      return result;
    }

    carve::poly::Polyhedron* genBox(const Rect3d& rect) {
      carve::input::PolyhedronData data;

      data.addVertex(carve::geom::VECTOR(rect.min(0), rect.min(1), rect.min(2)));
      data.addVertex(carve::geom::VECTOR(rect.min(0), rect.min(1), rect.max(2)));
      data.addVertex(carve::geom::VECTOR(rect.min(0), rect.max(1), rect.min(2)));
      data.addVertex(carve::geom::VECTOR(rect.min(0), rect.max(1), rect.max(2)));
      data.addVertex(carve::geom::VECTOR(rect.max(0), rect.min(1), rect.min(2)));
      data.addVertex(carve::geom::VECTOR(rect.max(0), rect.min(1), rect.max(2)));
      data.addVertex(carve::geom::VECTOR(rect.max(0), rect.max(1), rect.min(2)));
      data.addVertex(carve::geom::VECTOR(rect.max(0), rect.max(1), rect.max(2)));

      data.addFace(0, 1, 3, 2);
      data.addFace(7, 5, 4, 6);
      data.addFace(0, 2, 6, 4);
      data.addFace(2, 3, 7, 6);
      data.addFace(3, 1, 5, 7);
      data.addFace(1, 0, 4, 5);
      
      carve::poly::Polyhedron* result = new carve::poly::Polyhedron(data.points, data.getFaceCount(), data.faceIndices);
      result->canonicalize();
      return result;
    }

  } // namespace

// -------------------------------------------------------------------------- //
// LoopFormationExtruder
// -------------------------------------------------------------------------- //
  carve::poly::Polyhedron* LoopFormationExtruder::operator() () {
    carve::poly::Polyhedron* result = NULL;

    /* TODO: we better move this to LoopFormationConstructor and add Class enum to LoopFormation (i.e. NORMAL, SPHERE, etc...) */
    if(LoopUtils::isSphere(mLoopFormation, 1.0e-6)) { /* TODO: EPS */
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
    } else if(mLoopFormation->clazz() == LoopFormation::CONE) {
      const LoopFormation::Cone& cone = mLoopFormation->asCone();
      result = genCone(cone.base, cone.height, cone.baseX, cone.baseY, cone.topX, cone.topY);

      if(mLoopFormation->type() == LoopFormation::PROTRUSION) {
        Rect3d rect;
        rect.extend(cone.base + cone.baseX + cone.baseY);
        rect.extend(cone.base - cone.baseX - cone.baseY);
        rect.extend(cone.base + cone.height - cone.topX - cone.topY);
        rect.extend(cone.base + cone.height + cone.topX + cone.topY);

        carve::poly::Polyhedron* outerPoly = genBox(rect);

        carve::csg::CSG_TreeNode* node = 
          new carve::csg::CSG_OPNode(
            new carve::csg::CSG_PolyNode(outerPoly, true),
            new carve::csg::CSG_PolyNode(result, false),
            carve::csg::CSG::A_MINUS_B,
            true
          );

        carve::csg::CSG csg;
        carve::poly::Polyhedron* diffPoly = node->eval(csg);
        diffPoly->canonicalize();
        mCorrections.push_back(diffPoly);
      }
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

    if(result != NULL) {
      result->canonicalize();
      //debugShowPoly(result);
    }
    return result;
  }

} // namespace qr
