#include "RelationConstructor.h"
#include <boost/foreach.hpp>

namespace qr {
// -------------------------------------------------------------------------- //
// RelationConstructor
// -------------------------------------------------------------------------- //
  void RelationConstructor::operator() () {
    /* Construct parallel / perpendicular size correspondence edges. */
    foreach(View* aView, mViews) {
      foreach(View* bView, mViews) {
        if(aView == bView)
          continue;

        /* Determine whether the line segment that connects the centre points 
         * of a and b traverses the bounding boxes of any other nodes */
        bool intersects = false;
        Segment2d segment(aView->center(), bView->center());
        foreach(View* cView, mViews) {
          if(cView == aView || cView == bView)
            continue;

          if(segment.intersects(cView->boundingRect(), mPrec)) {
            intersects = true;
            break;
          }
        }

        /* TODO: continue if intersects? */

        /* Add parallel / perpendicular size correspondence arcs. */
        if(!intersects) {
          Vector2d aSize = aView->boundingRect().size();
          Vector2d bSize = bView->boundingRect().size();

          if(std::abs(aSize[0] - bSize[0]) < mPrec)
            aView->add(new ViewRelation(ViewRelation::PARALLEL,      aView, bView, ViewRelation::X));
          if(std::abs(aSize[0] - bSize[1]) < mPrec)
            aView->add(new ViewRelation(ViewRelation::PERPENDICULAR, aView, bView, ViewRelation::X));
          if(std::abs(aSize[1] - bSize[1]) < mPrec)
            aView->add(new ViewRelation(ViewRelation::PARALLEL,      aView, bView, ViewRelation::Y));
          if(std::abs(aSize[1] - bSize[0]) < mPrec)
            aView->add(new ViewRelation(ViewRelation::PERPENDICULAR, aView, bView, ViewRelation::Y));
        }

        /* Searching for name correspondence. */
        if(bView->type() == View::SECTIONAL) {
          foreach(CuttingChain* cuttingChain, aView->cuttingChains()) {
            if(bView->name() == cuttingChain->name()) {
              assert(bView->type() == View::SECTIONAL);
              bView->setSourceCuttingChain(cuttingChain);

              aView->add(new ViewRelation(ViewRelation::NAME, aView, bView, ViewRelation::directionOf(cuttingChain->segment(0)->asSegment().asLine().direction())));
            }
          }
        }

        /* Search for center correspondence. */
        bool centerFound = false;
        foreach(Edge* segment, aView->segments(Edge::NORMAL)) {
          if(segment->type() != Edge::ARC)
            continue;
          
          const Edge::ArcData& arc = segment->asArc();
          foreach(Edge* segment, bView->segments(Edge::CENTER)) {
            if(segment->asSegment().asLine().contains(arc.center(), mPrec)) {
              aView->add(new ViewRelation(ViewRelation::CENTER, aView, bView, ViewRelation::perpendicularDirection(ViewRelation::directionOf(segment->asSegment().asLine().direction()))));
              centerFound = true;
            }
          }
          if(centerFound)
            break;
        }
      }
    }
  }

} // namespace qr
