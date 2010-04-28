#include "LoopFormationExtruder.h"
#include <algorithm> /* for std::random_shuffle() */
#include <boost/foreach.hpp>
#include <carve/csg.hpp>
#include <carve/tree.hpp>
#include "LoopExtruder.h"

namespace qr {
// -------------------------------------------------------------------------- //
// LoopFormationExtruder
// -------------------------------------------------------------------------- //
  carve::poly::Polyhedron* LoopFormationExtruder::operator() () {
    for(int i = 0; i < mAttempts; i++) {
      std::random_shuffle(mLoops.begin(), mLoops.end());
      carve::csg::CSG_TreeNode* node = NULL;
      foreach(Loop* loop, mLoops) {
        carve::poly::Polyhedron* extrudedLoop = LoopExtruder(loop)();
        if(node == NULL)
          node = new carve::csg::CSG_PolyNode(extrudedLoop, true);
        else
          node = new carve::csg::CSG_OPNode(new carve::csg::CSG_PolyNode(extrudedLoop, true), node, carve::csg::CSG::INTERSECTION, true);
      }

      try {
        carve::csg::CSG csg;
        return node->eval(csg);
      } catch (carve::exception&) {}
    }

    return NULL;
  }

} // namespace qr
