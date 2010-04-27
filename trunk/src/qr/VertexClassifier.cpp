#include "VertexClassifier.h"

namespace qr {

  void VertexClassifier::operator() () {
    foreach(View* view, mViews) {
      foreach(Vertex* vertex, view->vertices()) {
        bool hasCuttingEdges = false;
        bool hasOtherEdges = false;
        foreach(Edge* edge, vertex->edges()) {
          if(edge->role() == Edge::CUTTING)
            hasCuttingEdges = true;
          else 
            hasOtherEdges = true;
        }

        /* TODO: split vertices in case there are not only cutting edges. */
        if(hasCuttingEdges)
          assert(!hasOtherEdges);

        if(hasCuttingEdges)
          vertex->setType(Vertex::VIRTUAL);
      }
    }
  }

} // namespace qr
