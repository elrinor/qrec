#include "LoopConstructor.h"
#include <limits>
#include <algorithm> /* for std::swap() */
#include <set>
#include <QSet>

namespace qr {
  namespace {
    Loop* traceLoop(Edge* startEdge, Vertex* startVertex, double* sumAngle, double prec) {
      Vertex* endVertex = startEdge->otherVertex(startVertex);

      Loop* loop = new Loop();
      loop->addEdge(startEdge);

      Edge* edge = startEdge;
      Vertex* vertex = startVertex;
      *sumAngle = 0.0;

      while(true) {
        /*std::vector<Edge*> edges;
        std::copy(loop->edges().begin(), loop->edges().end(), std::back_inserter(edges));

        if((vertex->pos2d() - Vector2d(0.0, 66.0)).isZero(0.001)) {
          int x  =1;
        }*/

        Edge* nextEdge = NULL;
        double minAngle = std::numeric_limits<double>::max();
        foreach(Edge* otherEdge, edge->extensions(vertex->pos2d(), prec)) {
          if(otherEdge->role() != Edge::PHANTOM && otherEdge->role() != Edge::NORMAL)
            continue;

          Vector2d edgeDir = (edge->otherEnd(vertex->pos2d(), prec) - vertex->pos2d()).normalized();
          Vector2d otherEdgeDir = (otherEdge->otherEnd(vertex->pos2d(), prec) - vertex->pos2d()).normalized();

          Vector3d cross = Vector3d(otherEdgeDir.x(), otherEdgeDir.y(), 0.0).cross(Vector3d(edgeDir.x(), edgeDir.y(), 0.0));

          double angle = acos(std::max(-1.0, std::min(1.0, edgeDir.dot(otherEdgeDir))));
          if(cross.z() < 0)
            angle = -angle + 2 * M_PI;

          if(angle < minAngle) {
            minAngle = angle;
            nextEdge = otherEdge;
          }
        }
          
        *sumAngle += minAngle;
        if(nextEdge == NULL)
          break;
        
        loop->addEdge(nextEdge);
        edge = nextEdge;
        vertex = nextEdge->otherVertex(vertex);

        if(vertex == endVertex)
          break;
      }

      if(vertex == endVertex) {
        bool isSolid = true;
        foreach(Edge* edge, loop->edges())
          if(edge->role() == Edge::PHANTOM)
            isSolid = false;
        loop->setSolid(isSolid);

        /*QColor color = QColor(rand() * 255 / RAND_MAX, rand() * 255 / RAND_MAX, rand() * 255 / RAND_MAX);
        foreach(Edge* edge, loop->edges())
          edge->setPen(QPen(color));*/

        return loop;
      } else {
        delete loop;
        return NULL;
      }
    }

    bool isType2(Loop* loop, double sumAngle, double prec) {
      return sumAngle > (loop->edges().size() - 2) * M_PI + prec;
    }

    std::set<Edge*> edgeSet(Loop* loop) {
      std::set<Edge*> result;
      result.insert(loop->edges().begin(), loop->edges().end());
      return result;
    }

  } // namespace

// -------------------------------------------------------------------------- //
// LoopConstructor
// -------------------------------------------------------------------------- //
  void LoopConstructor::operator() () {
    foreach(View* view, mViews) {
      QSet<Edge*> visitedEdges;

      std::set<std::set<Edge*>> loopSet;

      /* Create outer loop. */
      Edge* outerEdge = NULL;
      double minX = std::numeric_limits<double>::max();
      foreach(Edge* edge, view->edges()) {
        double x = edge->boundingRect().center().x();
        if(x < minX) {
          minX = x;
          outerEdge = edge;
        }
      }
      double sumAngle = 0.0;
      Loop* outerLoop = traceLoop(outerEdge, outerEdge->vertex(0), &sumAngle, mPrec);
      if(!isType2(outerLoop, sumAngle, mPrec))
        outerLoop = traceLoop(outerEdge, outerEdge->vertex(1), &sumAngle, mPrec);
      outerLoop->reverse(); /* Turn it into type-1. */
      outerLoop->setFundamental(false);
      outerLoop->setSolid(true);
      outerLoop->setDisjoint(false);
      outerLoop->setHatched(false);
      view->add(outerLoop);
      view->setOuterLoop(outerLoop);
      loopSet.insert(edgeSet(outerLoop));

      /* Create fundamental loops. */
      while(true) {
        Edge* startEdge = NULL;
        foreach(Edge* otherEdge, view->edges()) {
          if((otherEdge->role() == Edge::PHANTOM || otherEdge->role() == Edge::NORMAL) && !visitedEdges.contains(otherEdge)) {
            startEdge = otherEdge;
            break;
          }
        }
        if(startEdge == NULL)
          break;

        visitedEdges.insert(startEdge);

        Vertex* startVertex = startEdge->vertex(1);
        double sumAngle = 0.0;

        Loop* loop = traceLoop(startEdge, startVertex, &sumAngle, mPrec);
        if(loop == NULL)
          continue;
        if(isType2(loop, sumAngle, mPrec)) {
          /* That's a type-2 loop, we don't need it yet. */
          delete loop; 
          loop = traceLoop(startEdge, startEdge->otherVertex(startVertex), &sumAngle, mPrec);
          if(loop == NULL)
            continue;
        }

        /* Check duplicates. */
        std::set<Edge*> loopEdges = edgeSet(loop);
        if(loopSet.find(loopEdges) != loopSet.end()) {
          delete loop;
          continue;
        }
        loopSet.insert(loopEdges);

        loop->setFundamental(true);
        view->add(loop);
        foreach(Edge* edge, loop->edges())
          visitedEdges.insert(edge);
      }

      /* Register loops in edges. */
      foreach(Loop* loop, view->loops())
        foreach(Edge* edge, loop->edges())
          edge->addLoop(loop);

      /* Find disjoint loops. */
      foreach(Loop* loop, view->loops()) {
        bool isDisjoint = true;
        foreach(Edge* edge, loop->edges()) {
          if(edge->loops().size() > 1) {
            isDisjoint = false;
          }
        }
        loop->setDisjoint(isDisjoint);
      }

      /* Mark hatched loops. */
      foreach(Loop* loop, view->loops()) {
        if(loop->view()->type() != View::SECTIONAL)
          continue;

        bool isHatched = true;
        Hatch* hatch = NULL;
        foreach(Edge* edge, loop->edges()) {
          if(edge->hatch() != NULL) {
            if(hatch == NULL) {
              hatch = edge->hatch();
            } else if(hatch != edge->hatch()) {
              isHatched = false;
              break;
            }
          } else {
            isHatched = false;
            break;
          }
        }
        loop->setHatched(isHatched);
      }
    }
  }

} // namespace qr
