#include "ObjectConstructor.h"
#include <algorithm> /* for std::swap() */
#include <iterator> /* for std::back_inserter() */
#include <QHash>
#include <carve/csg.hpp>
#include <carve/tree.hpp>
#include "LoopFormationExtruder.h"
#include "Debug.h"

namespace qr {
// -------------------------------------------------------------------------- //
// ObjectConstructor
// -------------------------------------------------------------------------- //
  carve::poly::Polyhedron* ObjectConstructor::operator() () {
    QList<LoopFormation*> loopFormations;
    std::copy(mViewBox->loopFormations().begin(), mViewBox->loopFormations().end(), std::back_inserter(loopFormations));
    //std::random_shuffle(loopFormations.begin(), loopFormations.end());

    QList<carve::poly::Polyhedron*> corrections;

    QHash<LoopFormation*, carve::poly::Polyhedron*> formationPolygons;
    foreach(LoopFormation* loopFormation, loopFormations)
      formationPolygons[loopFormation] = LoopFormationExtruder(loopFormation, mAttempts, corrections)();

    /* Protrusions go first. */
    for(int i = 0; i < loopFormations.size(); i++)
      for(int j = i + 1; j < loopFormations.size(); j++)
        if(loopFormations[i]->type() == LoopFormation::DEPRESSION && loopFormations[j]->type() == LoopFormation::PROTRUSION)
          std::swap(loopFormations[i], loopFormations[j]);

    carve::poly::Polyhedron* result = NULL;

    foreach(LoopFormation* loopFormation, loopFormations) {
      carve::poly::Polyhedron* poly = formationPolygons[loopFormation];

      //debugSavePoly(correction, loopFormation->type() == LoopFormation::PROTRUSION ? carve::csg::CSG::UNION : carve::csg::CSG::A_MINUS_B);

      if(poly == NULL)
        continue;
      if(result == NULL) {
        result = poly;
      } else {
        carve::csg::CSG_TreeNode* lNode = new carve::csg::CSG_PolyNode(result, true);
        carve::csg::CSG_TreeNode* rNode = new carve::csg::CSG_PolyNode(poly, true);
        carve::csg::CSG::OP op = loopFormation->type() == LoopFormation::PROTRUSION ? carve::csg::CSG::UNION : carve::csg::CSG::A_MINUS_B;
        carve::csg::CSG_TreeNode* node = new carve::csg::CSG_OPNode(lNode, rNode, op, true);

        carve::csg::CSG csg;
        result = node->eval(csg);
        result->canonicalize();
      }
    }

    //debugShowPoly(result);

    foreach(carve::poly::Polyhedron* correction, corrections) {
      carve::csg::CSG_TreeNode* lNode = new carve::csg::CSG_PolyNode(result, true);
      carve::csg::CSG_TreeNode* rNode = new carve::csg::CSG_PolyNode(correction, true);
      carve::csg::CSG::OP op = carve::csg::CSG::A_MINUS_B;
      carve::csg::CSG_TreeNode* node = new carve::csg::CSG_OPNode(lNode, rNode, op, true);

      //debugShowPoly(correction);

      carve::csg::CSG csg;
      result = node->eval(csg);
      result->canonicalize();
    }

    return result;
  }

} // namespace qr
