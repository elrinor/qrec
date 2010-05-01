#include "LoopFormationExtruder.h"
#include <algorithm> /* for std::random_shuffle() */
#include <iterator> /* for std::back_inserter() */
#include <boost/foreach.hpp>
#include <carve/csg.hpp>
#include <carve/tree.hpp>
#include "LoopExtruder.h"
#include "Debug.h"

namespace qr {
// -------------------------------------------------------------------------- //
// LoopFormationExtruder
// -------------------------------------------------------------------------- //
  carve::poly::Polyhedron* LoopFormationExtruder::operator() () {
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
        carve::poly::Polyhedron* result = node->eval(csg);
        result->canonicalize();
        //debugShowPoly(result);
        return result;
      } catch (carve::exception&) {}
      
      std::random_shuffle(loops.begin(), loops.end());
    }

    return NULL;
  }

} // namespace qr
