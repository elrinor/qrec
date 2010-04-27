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

    /* Replace hatch segments with their real counterparts. */
    QSet<Edge*> unusedSegments;
    foreach(Hatch* hatch, mDrawing->hatches()) {
      foreach(Edge* hatchSegment, hatch->segments()) {
        foreach(Edge* segment, mDrawing->segments()) {
          if(segment->hatch() != NULL)
            continue;

          if(hatchSegment->isCoincident(segment, mPrec)) {
            segment->setHatch(hatch);
            unusedSegments.insert(hatchSegment);
            hatch->replaceSegment(hatchSegment, segment);
          }
        }
      }
    }

    /* Delete unused segments. */
    QList<Edge*> newSegments;
    foreach(Edge* segment, mDrawing->segments()) {
      if(unusedSegments.contains(segment))
        delete segment;
      else
        newSegments.push_back(segment);
    }
    mDrawing->setSegments(newSegments);

    /* Add extensions. */
    foreach(Edge* aSeg, mDrawing->segments())
      foreach(Edge* bSeg, mDrawing->segments())
        if(aSeg != bSeg && aSeg->role() == bSeg->role())
          if(aSeg->isExtension(bSeg, mPrec))
            aSeg->addExtension(bSeg, mPrec);
  }

} // namespace qr

