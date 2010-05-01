#include "RelationFilter.h"
#include <set>
#include <vector>
#include <limits>
#include <functional> /* for std::greater */
#include <utility> /* for std::pair */
#include <algorithm> /* for std::swap, std::sort */
#include <QSet>
#include "EvidenceSet.h"

namespace qr {
  namespace {
    typedef std::set<std::pair<View*, View*> > container_type;
    typedef EvidenceSet<std::pair<View*, View*>> evidence_set_type;

    static container_type singletonSet(View* a, View* b) {
      assert(a->id() < b->id());

      container_type result;
      result.insert(std::make_pair(a, b));
      return result;
    }

  } // namespace

  /* TODO: rewrite this evil magic? */

// -------------------------------------------------------------------------- //
// RelationFilter
// -------------------------------------------------------------------------- //
  void RelationFilter::operator() () {
    container_type everything;
    foreach(View* aView, mViews)
      foreach(View* bView, mViews)
        if(aView->id() < bView->id())
          everything.insert(std::make_pair(aView, bView));

    evidence_set_type evidenceSet;
    evidenceSet.addEvidence(everything, 1);

    foreach(View* view, mViews) {
      foreach(ViewRelation* relation, view->relations()) {
        View* aView = view;
        View* bView = relation->target();

        if(aView->id() >= bView->id())
          std::swap(aView, bView);

        double belief;
        if(relation->type() == ViewRelation::PARALLEL || relation->type() == ViewRelation::PERPENDICULAR || relation->type() == ViewRelation::CENTER) {
          belief = 0.3;
        } else {
          assert(relation->type() == ViewRelation::NAME);
          belief = 0.8;
        }
        relation->setBelief(belief);

        evidence_set_type newEvidenceSet;
        container_type single = singletonSet(aView, bView);
        if(single != everything) {
          newEvidenceSet.addEvidence(everything, 1 - belief);
          newEvidenceSet.addEvidence(single, belief);
        } else {
          newEvidenceSet.addEvidence(everything, 1);
        }
        evidenceSet.combine(newEvidenceSet);
      }
    }

    std::vector<std::pair<double, std::pair<View*, View*> > > beliefs;
    foreach(View* aView, mViews)
      foreach(View* bView, mViews)
        if(aView->id() < bView->id())
          beliefs.push_back(std::make_pair(evidenceSet.evidence(singletonSet(aView, bView)), std::make_pair(aView, bView)));
    std::sort(beliefs.begin(), beliefs.end(), std::greater<std::pair<double, std::pair<View*, View*> > >());

    QSet<View*> nodes;
    std::set<std::pair<View*, View*> > edges;
    std::pair<View*, View*> maxBeliefEdge = beliefs[0].second;
    double maxBelief = beliefs[0].first;
    while(true) {
      nodes.insert(maxBeliefEdge.first);
      nodes.insert(maxBeliefEdge.second);
      edges.insert(maxBeliefEdge);
      maxBeliefEdge.first->addAdjacentView(maxBeliefEdge.second/*, maxBelief*/);
      maxBeliefEdge.second->addAdjacentView(maxBeliefEdge.first/*, maxBelief*/);

      if(nodes.size() == mViews.size())
        break;

      maxBelief = -std::numeric_limits<double>::max();
      for(unsigned i = 0; i < beliefs.size(); i++) {
        View* aView = beliefs[i].second.first;
        View* bView = beliefs[i].second.second;
        if((nodes.contains(aView) && nodes.contains(bView)) || (!nodes.contains(aView) && !nodes.contains(bView)))
          continue;

        double belief = beliefs[i].first;
        if(belief > maxBelief) {
          maxBelief = belief;
          maxBeliefEdge = beliefs[i].second;
        }
      }
    }

    foreach(View* view, mViews) {
      QList<ViewRelation*> droppedRelations;
      foreach(ViewRelation* relation, view->relations()) {
        View* aView = view;
        View* bView = relation->target();

        if(aView->id() >= bView->id())
          std::swap(aView, bView);

        if(edges.find(std::make_pair(aView, bView)) == edges.end())
          droppedRelations.push_back(relation);
      }
      foreach(ViewRelation* relation, droppedRelations)
        view->remove(relation);
    }
  }

} // namespace qr
