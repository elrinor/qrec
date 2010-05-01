#include "ObjectConstructor.h"
#include <algorithm> /* for std::swap() */
#include <iterator> /* for std::back_inserter() */
#include <QHash>
#include <carve/csg.hpp>
#include <carve/tree.hpp>
#include "LoopFormationExtruder.h"

namespace qr {
// -------------------------------------------------------------------------- //
// ObjectConstructor
// -------------------------------------------------------------------------- //
  carve::poly::Polyhedron* ObjectConstructor::operator() () {
    QList<LoopFormation*> loopFormations;
    std::copy(mViewBox->loopFormations().begin(), mViewBox->loopFormations().end(), std::back_inserter(loopFormations));

    QHash<LoopFormation*, carve::poly::Polyhedron*> formationPolygons;
    foreach(LoopFormation* loopFormation, loopFormations)
      formationPolygons[loopFormation] = LoopFormationExtruder(loopFormation, mAttempts)();

    /* Protrusions go first. */
    for(int i = 0; i < loopFormations.size(); i++)
      for(int j = i + 1; j < loopFormations.size(); j++)
        if(loopFormations[i]->type() == LoopFormation::DEPRESSION && loopFormations[j]->type() == LoopFormation::PROTRUSION)
          std::swap(loopFormations[i], loopFormations[j]);

    /*for(int i = 0; i < mAttempts; i++) {
      std::random_shuffle(loopFormations.begin(), loopFormations.end());*/
      carve::csg::CSG_TreeNode* node = NULL;
      foreach(LoopFormation* loopFormation, loopFormations) {
        carve::poly::Polyhedron* poly = formationPolygons[loopFormation];
        if(node == NULL)
          node = new carve::csg::CSG_PolyNode(poly, true);
        else {
          carve::csg::CSG_TreeNode* rNode = new carve::csg::CSG_PolyNode(poly, true);
          carve::csg::CSG::OP op = loopFormation->type() == LoopFormation::PROTRUSION ? carve::csg::CSG::UNION : carve::csg::CSG::A_MINUS_B;
          /*if(op == carve::csg::CSG::A_MINUS_B) {
            carve::math::Matrix trans1 = carve::math::Matrix::TRANS(-poly->aabb.pos);
            carve::math::Matrix trans2 = carve::math::Matrix::TRANS(poly->aabb.pos);
            carve::math::Matrix scale = carve::math::Matrix::SCALE(1.001, 1.001, 1.001);
            rNode = new carve::csg::CSG_TransformNode(trans2 * scale * trans1, rNode);
          }*/
          
          node = new carve::csg::CSG_OPNode(node, rNode, op, true);
        }
      }

      //try {
        carve::csg::CSG csg;
        return node->eval(csg);
      /*} catch (carve::exception&) {}
    }

    return NULL;*/

    //return formationPolygons[loopFormations[loopFormations.size() - 1]];
  }

} // namespace qr
