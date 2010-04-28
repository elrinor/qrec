#include "LoopConstructor.h"
#include <limits>
#include <algorithm> /* for std::swap() */
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
        Edge* nextEdge = NULL;
        double minAngle = std::numeric_limits<double>::max();
        foreach(Edge* otherEdge, edge->extensions(vertex->pos2d(), prec)) {
          if(otherEdge->role() != Edge::PHANTOM && otherEdge->role() != Edge::NORMAL)
            continue;

          Vector2d edgeDir = (edge->otherEnd(vertex->pos2d(), prec) - vertex->pos2d()).normalized();
          Vector2d otherEdgeDir = (otherEdge->otherEnd(vertex->pos2d(), prec) - vertex->pos2d()).normalized();

          Vector3d cross = Vector3d(otherEdgeDir.x(), otherEdgeDir.y(), 0.0).cross(Vector3d(edgeDir.x(), edgeDir.y(), 0.0));

          double angle = acos(edgeDir.dot(otherEdgeDir));
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

        /*
        QColor color = QColor(rand() * 255 / RAND_MAX, rand() * 255 / RAND_MAX, rand() * 255 / RAND_MAX);
        foreach(Edge* edge, loop->edges())
          edge->setPen(QPen(color));
        */

        return loop;
      } else {
        delete loop;
        return NULL;
      }
    }

    bool isType2(Loop* loop, double sumAngle) {
      return sumAngle > loop->edges().size() * M_PI;
    }

  } // namespace

// -------------------------------------------------------------------------- //
// LoopConstructor
// -------------------------------------------------------------------------- //
  void LoopConstructor::operator() () {
    foreach(View* view, mViews) {
      QSet<Edge*> visitedEdges;

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
      if(!isType2(outerLoop, sumAngle))
        outerLoop = traceLoop(outerEdge, outerEdge->vertex(1), &sumAngle, mPrec);
      outerLoop->setFundamental(false);
      outerLoop->setSolid(true);
      outerLoop->setDisjoint(false);
      view->add(outerLoop);
      view->setOuterLoop(outerLoop);

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
        if(isType2(loop, sumAngle)) {
          /* That's a type-2 loop, we don't need it yet. */
          delete loop; 
          loop = traceLoop(startEdge, startEdge->otherVertex(startVertex), &sumAngle, mPrec);
          if(loop == NULL)
            continue;
        }

        loop->setFundamental(true);
        view->add(loop);
        foreach(Edge* edge, loop->edges()) {
          visitedEdges.insert(edge);
          edge->addLoop(loop);
        }
      }

      /* Find disjoint loops. */
      foreach(Loop* loop, view->loops()) {
        bool isDisjoint = true;
        foreach(Edge* edge, loop->edges())
          if(edge->loops().size() > 1)
            isDisjoint = false;
        loop->setDisjoint(isDisjoint);
      }
    }
  }

} // namespace qr
