#include "Preprocessor.h"
#include <QList>
#include <QSet>

namespace qr {
// -------------------------------------------------------------------------- //
// Preprocessor
// -------------------------------------------------------------------------- //
  void Preprocessor::operator() () {
    /* TODO: 
     * We don't check for intersections here (yet). 
     * Normally we should first split all segments at intersection points, 
     * then construct neighbors. */

    QSet<Edge*> unusedEdges;

    /* Replace hatch segments with their real counterparts. */
    foreach(Hatch* hatch, mDrawing->hatches()) {
      foreach(Edge* hatchSegment, hatch->segments()) {
        foreach(Edge* segment, mDrawing->edges()) {
          if(segment->hatch() != NULL)
            continue;

          if(hatchSegment->isCoincident(segment, mPrec)) {
            segment->setHatch(hatch);
            unusedEdges.insert(hatchSegment);
            hatch->replaceSegment(hatchSegment, segment);
          }
        }
      }
    }

    /* Remove zero-length edges. */
    foreach(Edge* edge, mDrawing->edges())
      if(!unusedEdges.contains(edge) && (edge->end(0) - edge->end(1)).isZero(mPrec))
        unusedEdges.insert(edge);

    /* Delete unused edges. */
    QList<Edge*> newEdges;
    foreach(Edge* segment, mDrawing->edges()) {
      if(unusedEdges.contains(segment))
        delete segment;
      else
        newEdges.push_back(segment);
    }
    mDrawing->setEdges(newEdges);

    /* Add extensions. */
    foreach(Edge* aEdge, mDrawing->edges())
      foreach(Edge* bEdge, mDrawing->edges())
        if(aEdge != bEdge && (aEdge->role() == bEdge->role() || (aEdge->role() == Edge::NORMAL && bEdge->role() == Edge::PHANTOM) || (bEdge->role() == Edge::NORMAL && aEdge->role() == Edge::PHANTOM)))
          if(aEdge->isExtension(bEdge, mPrec))
            aEdge->addExtension(bEdge, mPrec);
  }

} // namespace qr

