#include "LoopMerger.h"
#include <set>
#include <algorithm> /* for std::copy() */
#include <iterator> /* for std::inserter */
#include <boost/foreach.hpp>

namespace qr {
// -------------------------------------------------------------------------- //
// LoopMerger
// -------------------------------------------------------------------------- //
  Loop* LoopMerger::operator() () {
    std::set<Edge*> firstEdges, secondEdges;

    std::copy(mFirst->edges().begin(), mFirst->edges().end(), std::inserter(firstEdges, firstEdges.begin()));
    std::copy(mSecond->edges().begin(), mSecond->edges().end(), std::inserter(secondEdges, secondEdges.begin()));

    std::set<Edge*> commonEdges;
    std::set_intersection(firstEdges.begin(), firstEdges.end(), secondEdges.begin(), secondEdges.end(), std::inserter(commonEdges, commonEdges.begin()));
    if(commonEdges.empty())
      return NULL;

    std::set<Edge*> newEdges;
    std::set_union(firstEdges.begin(), firstEdges.end(), secondEdges.begin(), secondEdges.end(), std::inserter(newEdges, newEdges.begin()));
    foreach(Edge* edge, commonEdges)
      newEdges.erase(edge);

    Edge* startEdge = *newEdges.begin();
    Vertex* startVertex = startEdge->vertex(0);
    newEdges.erase(startEdge);

    Loop* loop = new Loop();
    Vertex* vertex = startVertex;

    while(true) {
      Edge* nextEdge = NULL;
      foreach(Edge* edge, newEdges) {
        if(edge->hasVertex(vertex)) {
          nextEdge = edge;
          break;
        }
      }
      if(nextEdge == NULL)
        return NULL;

      vertex = nextEdge->otherVertex(vertex);
      newEdges.erase(nextEdge);
      loop->addEdge(nextEdge);
      if(newEdges.empty()) {
        loop->setFundamental(false);
        loop->setDisjoint(false);
        
        bool isSolid = true;
        foreach(Edge* edge, loop->edges())
          if(edge->role() == Edge::PHANTOM)
            isSolid = false;
        loop->setSolid(isSolid);

        return loop;
      }
    }

    return NULL;
  }

} // namespace qr
