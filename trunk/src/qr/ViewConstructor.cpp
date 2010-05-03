#include "ViewConstructor.h"
#include <algorithm>
#include <limits>
#include "GRect.h"

namespace qr {
  namespace detail {
    struct ConnectedComponent {
      EIGEN_MAKE_ALIGNED_OPERATOR_NEW;

      QSet<Edge*> edges;
      Rect2d boundingRect;
    };
  }

// -------------------------------------------------------------------------- //
// ViewConstructor
// -------------------------------------------------------------------------- //
  QList<View*> ViewConstructor::operator() () {
    assert(mViews.empty());

    /* Create set of all normal edges. */
    QSet<Edge*> unprocessedEdges;
    foreach(Edge* edge, mDrawing->edges())
      if(edge->role() == Edge::NORMAL)
        unprocessedEdges.insert(edge);

    /* Construct connected components. */
    QList<detail::ConnectedComponent> components;
    while(!unprocessedEdges.empty()) {
      detail::ConnectedComponent component;

      QSet<Edge*> edges, newEdges;
      edges.insert(*unprocessedEdges.begin());
      unprocessedEdges.erase(unprocessedEdges.begin());

      do {
        foreach(Edge* edge, edges) {
          foreach(Edge* otherSegment, edge->extensions()) {
            if(unprocessedEdges.contains(otherSegment)) {
              unprocessedEdges.remove(otherSegment);
              newEdges.insert(otherSegment);
            }
          }
        }

        component.edges.unite(edges);
        edges = newEdges;
        newEdges.clear();
      } while (!edges.empty());

      components.push_back(component);
    }

    /* Calculate bounding boxes for components. */
    foreach(detail::ConnectedComponent& component, components)
      foreach(Edge* edge, component.edges)
        component.boundingRect.extend(edge->boundingRect());

    /* Merge components. */
    foreach(detail::ConnectedComponent& aComponent, components) {
      if(aComponent.edges.empty())
        continue;

      foreach(detail::ConnectedComponent& bComponent, components) {
        if(bComponent.edges.empty() || &aComponent == &bComponent)
          continue;

        if(aComponent.boundingRect.intersects(bComponent.boundingRect, mPrec)) { /* TODO: is intersects() OK here? */
          aComponent.edges.unite(bComponent.edges);
          bComponent.edges.clear();
        }
      }
    }

    /* Construct views. */
    int viewId = 0;
    foreach(detail::ConnectedComponent& component, components) {
      if(component.edges.empty())
        continue;

      View* view = new View(viewId++);
      foreach(Edge* edge, component.edges)
        view->add(edge);

      mViews.push_back(view);
    }

    /* Add other edges,... */
    foreach(Edge* edge, mDrawing->edges())
      if(edge->role() != Edge::NORMAL)
        closestView(edge->boundingRect().center())->add(edge);

    /* ...hatches,... */
    foreach(Hatch* hatch, mDrawing->hatches()) {
      View* view = closestView(hatch->boundingRect().center());
      view->add(hatch);
      view->setType(View::SECTIONAL);
    }

    /* ...and labels. */
    foreach(Label* label, mDrawing->labels()) {
      View* view = closestView(label->position());
      view->add(label);

      QString text = label->text().trimmed();
      if(text.size() == 3 && text[0] == text[2] && text[1] == '-')
        view->setName(QString(text[0]));
    }

    /* Trace cutting lines. */
    foreach(View* view, mViews) {
      QList<Edge*> edges = view->edges(Edge::CUTTING);
      while(!edges.empty()) {
        QList<Edge*> chain;
        bool chainValid = false;
        Edge* startEdge = NULL;
        foreach(Edge* edge, edges) {
          if(!view->boundingRect().contains(edge->asSegment(), mPrec)) {
            startEdge = edge;
            break;
          }
        }
        if(startEdge == NULL)
          break;
        chain.push_back(startEdge);
        edges.removeOne(startEdge);

        while(true) {
          Edge* endEdge = NULL;
          double minSquaredNorm = std::numeric_limits<double>::max();
          foreach(Edge* edge, edges) {
            if(!startEdge->asSegment().asLine().isCoincident(edge->asSegment().asLine(), mPrec))
              continue;

            double squaredNorm = (startEdge->boundingRect().center() - edge->boundingRect().center()).squaredNorm();
            if(squaredNorm < minSquaredNorm) {
              minSquaredNorm = squaredNorm;
              endEdge = edge;
            }
          }
          if(endEdge == NULL)
            break;
          chain.push_back(endEdge);
          edges.removeOne(endEdge);

          if(!view->boundingRect().contains(endEdge->asSegment(), mPrec)) {
            chainValid = true;
            break;
          }

          if(endEdge->extensions().size() != 1)
            break;

          startEdge = endEdge->extensions().back(); /* TODO: check whether we correctly calculate extensions for central lines */
          chain.push_back(startEdge);
          edges.removeOne(startEdge);
        }

        QString name;
        double minSquaredNorm = std::numeric_limits<double>::max();
        foreach(Label* label, view->labels()) {
          if(label->text().size() != 1)
            continue;

          double squaredNorm = (label->position() - chain[0]->boundingRect().center()).squaredNorm();
          if(squaredNorm < minSquaredNorm) {
            minSquaredNorm = squaredNorm;
            name = label->text();
          }
        }
        if(name.isEmpty())
          chainValid = false;

        if(chainValid) {
          assert(chain.size() == 2);
          foreach(Edge* edge, chain)
            view->remove(edge);

          CuttingChain* cuttingChain = new CuttingChain(name);
          for(int i = 0; i < chain.size(); i += 2) {
            Edge* cuttingEdge = new Edge(
              Edge::Line(), 
              chain[i]->asSegment().farthestEnd(chain[i + 1]->boundingRect().center()),
              chain[i + 1]->asSegment().farthestEnd(chain[i]->boundingRect().center()),
              chain[i]->color(),
              chain[i]->style()
              );
            cuttingEdge->setRole(Edge::CUTTING);
            cuttingChain->addEdge(cuttingEdge);
          }

          view->add(cuttingChain);
        }
      }
    }

    return mViews;
  }

  View* ViewConstructor::closestView(const Vector2d& point) const {
    double minSquaredNorm = std::numeric_limits<double>::max();
    View* result = NULL;
    foreach(View* view, mViews) {
      double squaredNorm = (point - view->center()).squaredNorm();
      if(squaredNorm < minSquaredNorm) {
        minSquaredNorm = squaredNorm;
        result = view;
      }
    }
    return result;
  }


} // namespace qr
