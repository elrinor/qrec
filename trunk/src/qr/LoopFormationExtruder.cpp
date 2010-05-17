#include "LoopFormationExtruder.h"
#include <algorithm> /* for std::random_shuffle() */
#include <iterator> /* for std::back_inserter() */
#include <boost/foreach.hpp>
#include <QSet>
#include <carve/csg.hpp>
#include <carve/tree.hpp>
#include <carve/input.hpp>
#include "LoopExtruder.h"
#include "Debug.h"
#include "View.h"
#include "LoopUtils.h"
#include "ViewBox.h"

namespace qr {
  namespace { 
    carve::geom::vector<3> toVECTOR(const Vector3d& v) {
      return carve::geom::VECTOR(v.x(), v.y(), v.z());
    }

    Vector3d to3d(const Vector2d& v) {
      return Vector3d(v.x(), v.y(), 0.0);
    }

    Vector3d spherePoint(const Vector3d& center, double radius, double phi, double psi) {
      return center + radius * Vector3d(cos(phi) * cos(psi), sin(phi) * cos(psi), sin(psi));
    }

    carve::poly::Polyhedron* genSphere(const Vector3d& center, double radius) {
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

      carve::poly::Polyhedron* result = new carve::poly::Polyhedron(data.points, data.getFaceCount(), data.faceIndices);
      result->canonicalize();
      return result;
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

    carve::poly::Polyhedron* genPrism(const Vector3d& origin, const Vector3d& height, const Vector3d& a, const Vector3d& b) {
      if(a.cross(b).dot(height) < 0)
        return genPrism(origin, height, b, a);

      carve::input::PolyhedronData data;

      data.addVertex(toVECTOR(origin));
      data.addVertex(toVECTOR(origin + a));
      data.addVertex(toVECTOR(origin + b));
      data.addVertex(toVECTOR(origin + height));
      data.addVertex(toVECTOR(origin + height + a));
      data.addVertex(toVECTOR(origin + height + b));

      data.addFace(2, 1, 0);
      data.addFace(0, 3, 5, 2);
      data.addFace(2, 5, 4, 1);
      data.addFace(1, 4, 3, 0);
      data.addFace(3, 4, 5);

      carve::poly::Polyhedron* result = new carve::poly::Polyhedron(data.points, data.getFaceCount(), data.faceIndices);
      result->canonicalize();
      return result;
    }

    carve::poly::Polyhedron* genArcWrapper(const QSet<Edge*>& edges) {
      assert(!edges.empty());
      foreach(Edge* edge, edges)
        assert(edge->type() == Edge::ARC && (edge->asArc().center() - (*edges.begin())->asArc().center()).isZero(1.0e-6)); /* TODO: EPS */

      Edge* edge = *edges.begin();
      int idx = edge->view()->perpendicularAxisIndex();
      double lo = edge->view()->viewBox()->boundingRect().min(idx);
      double hi = edge->view()->viewBox()->boundingRect().max(idx);
      
      Vector3d height = Vector3d(0.0, 0.0, 0.0);
      height[idx] = hi - lo;

      Vector3d origin = edge->view()->transform() * to3d(edge->asArc().center());
      origin[idx] = lo;

      carve::poly::Polyhedron* result = NULL;
      foreach(Edge* edge, edges) {
        Vector3d a = edge->vertex(0)->pos3d();
        Vector3d b = edge->vertex(1)->pos3d();
        a[idx] = lo;
        b[idx] = lo;
        a = a - origin;
        b = b - origin;

        carve::poly::Polyhedron* poly = genPrism(origin, height, 3 * a, 3 * b);
        if(result == NULL) {
          result = poly;
        } else {
          carve::csg::CSG_TreeNode* lNode = new carve::csg::CSG_PolyNode(result, true);
          carve::csg::CSG_TreeNode* rNode = new carve::csg::CSG_PolyNode(poly, true);
          carve::csg::CSG::OP op = carve::csg::CSG::UNION;
          carve::csg::CSG_TreeNode* node = new carve::csg::CSG_OPNode(lNode, rNode, op, true);

          carve::csg::CSG csg;
          result = node->eval(csg);
          result->canonicalize();
        }
      }

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

      result = genSphere(center, radius);
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

      QList<carve::poly::Polyhedron*> additions;

      /* Find potential spheres. */
      std::set<std::set<Edge*>> spheres;
      foreach(Loop* loop, mLoopFormation->loops()) {
        QSet<Edge*> usedEdges;

        foreach(Edge* edge, loop->edges()) {
          if(edge->type() != Edge::ARC)
            continue;

          if(usedEdges.contains(edge))
            continue;

          Vector2d center = edge->asArc().center();
          Vector3d center3d = loop->view()->transform() * to3d(edge->asArc().center());

          QSet<Edge*> edges;
          foreach(Edge* otherEdge, loop->edges()) {
            if(otherEdge->type() == Edge::ARC && (otherEdge->asArc().center() - center).isZero(1.0e-6)) { /* TODO: EPS */
              edges.insert(otherEdge);
              usedEdges.insert(otherEdge);
            }
          }

          foreach(Loop* otherLoop, mLoopFormation->loops()) {
            if(loop <= otherLoop)
              continue; /* Enforce ordering in a strange way =). */

            if(otherLoop->view()->perpendicularAxisIndex() == loop->view()->perpendicularAxisIndex())
              continue;

            int idx = 3 - loop->view()->perpendicularAxisIndex() - otherLoop->view()->perpendicularAxisIndex();

            QSet<Edge*> usedOtherEdges;
            foreach(Edge* otherEdge, otherLoop->edges()) {
              if(otherEdge->type() != Edge::ARC)
                continue;

              if(usedOtherEdges.contains(otherEdge))
                continue;

              if(std::abs(otherEdge->asArc().longAxis().norm() - edge->asArc().longAxis().norm()) > 1.0e-6) /* TODO: EPS */
                continue;

              Vector2d otherCenter = otherEdge->asArc().center();
              Vector3d otherCenter3d = otherLoop->view()->transform() * to3d(otherCenter);

              if(std::abs(center3d[idx] - otherCenter3d[idx]) > 1.0e-6) /* TODO: EPS */
                continue;

              QSet<Edge*> otherEdges;
              foreach(Edge* anotherEdge, otherLoop->edges()) {
                if(anotherEdge->type() == Edge::ARC && (anotherEdge->asArc().center() - otherCenter).isZero(1.0e-6)) { /* TODO: EPS */
                  otherEdges.insert(anotherEdge);
                  usedOtherEdges.insert(anotherEdge);
                }
              }

              /* Here edges & otherEdges form a spherical surface. */

              /* Check for duplicates. */
              std::set<Edge*> sphere;
              std::copy(edges.begin(), edges.end(), std::inserter(sphere, sphere.begin()));
              std::copy(otherEdges.begin(), otherEdges.end(), std::inserter(sphere, sphere.begin()));
              if(spheres.find(sphere) != spheres.end())
                continue;
              spheres.insert(sphere);

              /* Get center & radius. */                           
              Vector3d sphereCenter = center3d;
              sphereCenter[edge->view()->perpendicularAxisIndex()] = otherCenter3d[edge->view()->perpendicularAxisIndex()];

              double radius = edge->asArc().longAxis().norm();

              /* If we have more than two loops - check the 3rd one. */
              if(mLoopFormation->loops().size() > 2) {
                Loop* lastLoop = mLoopFormation->loop(0);
                if(lastLoop == loop || lastLoop == otherLoop)
                  lastLoop = mLoopFormation->loop(1);
                if(lastLoop == loop || lastLoop == otherLoop)
                  lastLoop = mLoopFormation->loop(2);

                bool hasLast = false;
                foreach(Edge* edge, lastLoop->edges()) {
                  if(edge->type() != Edge::ARC)
                    continue;

                  Vector3d lastCenter = lastLoop->view()->transform() * to3d(edge->asArc().center());

                  lastCenter[lastLoop->view()->perpendicularAxisIndex()] = sphereCenter[lastLoop->view()->perpendicularAxisIndex()];

                  if((lastCenter - sphereCenter).isZero(1.0e-6)) { /* TODO: EPS */
                    hasLast = true;
                    break;
                  }
                }
                if(!hasLast)
                  continue;
              }


              carve::poly::Polyhedron* firstWrapper = genArcWrapper(edges);
              carve::poly::Polyhedron* secondWrapper = genArcWrapper(otherEdges);

              carve::poly::Polyhedron* cube = genBox(Rect3d(center3d - Vector3d(radius, radius, radius), 2 * Vector3d(radius, radius, radius)));

              carve::csg::CSG_TreeNode* node0 = new carve::csg::CSG_OPNode(
                  new carve::csg::CSG_PolyNode(secondWrapper, true),
                  new carve::csg::CSG_PolyNode(firstWrapper, true),
                  carve::csg::CSG::INTERSECTION,
                  true
                );
              carve::csg::CSG csg0;
              carve::poly::Polyhedron* tmpPoly = node0->eval(csg0);
              tmpPoly->canonicalize();
              
              carve::csg::CSG_TreeNode* node1 =  new carve::csg::CSG_OPNode(
                  new carve::csg::CSG_PolyNode(cube, true),
                  new carve::csg::CSG_PolyNode(tmpPoly, true),
                  carve::csg::CSG::INTERSECTION,
                  true
                );
              carve::csg::CSG csg1;
              carve::poly::Polyhedron* boundingPoly = node1->eval(csg1);
              boundingPoly->canonicalize();

              /* Create result. */
              carve::poly::Polyhedron* spherePoly = genSphere(sphereCenter, radius);

              if(mLoopFormation->type() == LoopFormation::DEPRESSION) {
                carve::csg::CSG_TreeNode* otherNode = 
                  new carve::csg::CSG_OPNode(
                    new carve::csg::CSG_PolyNode(boundingPoly, false),
                    new carve::csg::CSG_PolyNode(spherePoly, false),
                    carve::csg::CSG::INTERSECTION,
                    true
                  );
                carve::csg::CSG csg;
                carve::poly::Polyhedron* addPoly = otherNode->eval(csg);
                addPoly->canonicalize();
                additions.push_back(addPoly);
              }

              if(mLoopFormation->type() == LoopFormation::PROTRUSION) {
                carve::csg::CSG_TreeNode* anotherNode = 
                  new carve::csg::CSG_OPNode(
                    new carve::csg::CSG_PolyNode(boundingPoly, false),
                    new carve::csg::CSG_PolyNode(spherePoly, false),
                    carve::csg::CSG::A_MINUS_B,
                    true
                  );
                carve::csg::CSG csg;
                carve::poly::Polyhedron* diffPoly = anotherNode->eval(csg);
                diffPoly->canonicalize();
                mCorrections.push_back(diffPoly);
              }

              delete boundingPoly;
              delete spherePoly;
            }
          }
        }
      }

      /* Put all loops with arcs in tail. */
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
          //debugSavePoly(extrudedLoop, carve::csg::CSG::INTERSECTION);
        }

        try {
          carve::csg::CSG csg;
          result = node->eval(csg);
          break;
        } catch (carve::exception&) {}

        std::random_shuffle(loops.begin(), loops.end());
      }

      if(result != NULL) {
        foreach(carve::poly::Polyhedron* addition, additions) {
          carve::csg::CSG_TreeNode* lNode = new carve::csg::CSG_PolyNode(result, true);
          carve::csg::CSG_TreeNode* rNode = new carve::csg::CSG_PolyNode(addition, true);
          carve::csg::CSG::OP op = carve::csg::CSG::UNION;
          carve::csg::CSG_TreeNode* node = new carve::csg::CSG_OPNode(lNode, rNode, op, true);

          //debugSavePoly(correction, op);

          carve::csg::CSG csg;
          result = node->eval(csg);
          result->canonicalize();
        }
      }
    }

    if(result != NULL) {
      result->canonicalize();
      //debugShowPoly(result);
    }
    return result;
  }

} // namespace qr
