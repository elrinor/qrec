#include "LoopFormationConstructor.h"
#include <set>
#include <boost/foreach.hpp>
#include "LoopMerger.h"

namespace qr {
  namespace {
    QList<double> loopPoints(Loop* loop, int idx, bool normalOnly) {
      QList<double> result;
      result.push_back(loop->boundingRect3d().min(idx));
      result.push_back(loop->boundingRect3d().max(idx));
      foreach(const LoopVertex& vertex, loop->vertices())
        if(vertex.type() == LoopVertex::NORMAL || !normalOnly)
          result.push_back(vertex.vertex()->pos3d(idx));
      return result;
    }

    bool matches(Loop* srcLoop, Loop* targetLoop, double prec) {
      assert(srcLoop->view()->perpendicularAxisIndex() != targetLoop->view()->perpendicularAxisIndex());

      int idx = 3 - srcLoop->view()->perpendicularAxisIndex() - targetLoop->view()->perpendicularAxisIndex();
  
      if(std::abs(srcLoop->boundingRect3d().min(idx) - targetLoop->boundingRect3d().min(idx)) > prec)
        return false;
      if(std::abs(srcLoop->boundingRect3d().max(idx) - targetLoop->boundingRect3d().max(idx)) > prec)
        return false;

      QList<double> aPoints = loopPoints(srcLoop, idx, true);
      QList<double> bPoints = loopPoints(targetLoop, idx, false);
      int aCount = 0, bCount = 0;

      foreach(double a, aPoints) {
        foreach(double b, bPoints) {
          if(std::abs(a - b) < prec) {
            aCount++;
            break;
          }
        }
      }
/*
      foreach(double b, bPoints) {
        foreach(double a, aPoints) {
          if(std::abs(a - b) < prec) {
            bCount++;
            break;
          }
        }
      }*/

      if(aCount == aPoints.size() || bCount == bPoints.size())
        return true;

      aPoints = loopPoints(srcLoop, idx, false);
      bPoints = loopPoints(targetLoop, idx, true);
/*      int aCount = 0, bCount = 0;

      foreach(double a, aPoints) {
        foreach(double b, bPoints) {
          if(std::abs(a - b) < prec) {
            aCount++;
            break;
          }
        }
      }*/

      foreach(double b, bPoints) {
        foreach(double a, aPoints) {
          if(std::abs(a - b) < prec) {
            bCount++;
            break;
          }
        }
      }

      if(aCount == aPoints.size() || bCount == bPoints.size())
        return true;

      return false;
    }

  } // namespace

// -------------------------------------------------------------------------- //
// LoopFormationConstructor
// -------------------------------------------------------------------------- //
  void LoopFormationConstructor::operator() () {
    QList<Loop*> allLoops;
    foreach(View* view, mViewBox->views())
      allLoops.append(view->loops());

    std::set<std::set<Loop*>> formationSet;

    foreach(Loop* aLoop, allLoops) {
      LoopFormation* loopFormation = new LoopFormation();
      loopFormation->addLoop(aLoop);
      
      foreach(Loop* bLoop, allLoops) {
        if(aLoop->view() == bLoop->view() || aLoop->view()->perpendicularAxisIndex() == bLoop->view()->perpendicularAxisIndex())
          continue;

        if(matches(aLoop, bLoop, mPrec))
          loopFormation->addLoop(bLoop);
      }

      if(loopFormation->loops().size() < 2) {
        delete loopFormation;
        continue;
      }

      bool hasDisjoint = false;
      foreach(Loop* loop, loopFormation->loops())
        if(loop->isDisjoint())
          hasDisjoint = true;
      if(!hasDisjoint && loopFormation->loops().size() < 3 && mViewBox->views().size() > 2) {
        delete loopFormation;
        continue;
      }

      QSet<View*> formationViews;
      bool hasDuplicates = false;
      foreach(Loop* loop, loopFormation->loops()) {
        if(formationViews.contains(loop->view())) {
          hasDuplicates = true;
          break;
        }
        formationViews.insert(loop->view());
      }
      if(hasDuplicates) {
        delete loopFormation;
        continue;
      }

      /* Check for duplicates. */
      std::set<Loop*> loopSet;
      foreach(Loop* loop, loopFormation->loops())
        loopSet.insert(loop);
      if(formationSet.find(loopSet) != formationSet.end()) {
        delete loopFormation;
        continue;
      }
      formationSet.insert(loopSet);

      mViewBox->addLoopFormation(loopFormation);

      /*QColor color = QColor(rand() * 255 / RAND_MAX, rand() * 255 / RAND_MAX, rand() * 255 / RAND_MAX);
      foreach(Loop* loop, loopFormation->loops())
        foreach(Edge* edge, loop->edges())
          edge->setPen(QPen(color));*/
    }

    /* Add bound-on-solid if needed. */
    std::set<Loop*> boundOnSolid;
    foreach(View* view, mViewBox->views())
      boundOnSolid.insert(view->outerLoop());
    if(formationSet.find(boundOnSolid) == formationSet.end()) {
      LoopFormation* loopFormation = new LoopFormation();
      foreach(View* view, mViewBox->views())
        loopFormation->addLoop(view->outerLoop());
      mViewBox->addLoopFormation(loopFormation);
    }

    /* Classify. */
    foreach(LoopFormation* loopFormation, mViewBox->loopFormations()) {
      bool classified = false;

      /* If we have a hatched loop then it's protrusion. */
      foreach(Loop* loop, loopFormation->loops()) {
        if(loop->view()->type() == View::SECTIONAL && loop->isHatched()) {
          loopFormation->setType(LoopFormation::PROTRUSION);
          classified = true;
          break;
        }
      }
      if(classified)
        continue;

      /* It it's outer loop - then it's protrusion. */
      foreach(Loop* loop, loopFormation->loops()) {
        if(loop == loop->view()->outerLoop()) {
          loopFormation->setType(LoopFormation::PROTRUSION);
          classified = true;
          break;
        }
      }
      if(classified)
        continue;

      /* If all vertices of target loop are on outer loop & merge successful then it's protrusion. */
      foreach(Loop* loop, loopFormation->loops()) {
        if(loop->view()->outerLoop()->vertexSet().contains(loop->vertexSet())) {
          Loop* merge = LoopMerger(loop, loop->view()->outerLoop())();
          if(merge != NULL) {
            delete merge;
            loopFormation->setType(LoopFormation::PROTRUSION);
            classified = true;
            break;
          } else {
            loopFormation->setType(LoopFormation::DEPRESSION);
            classified = true;
            break;
          }
        }
      }
      if(classified)
        continue;

      /* TODO */
      foreach(Loop* loop, loopFormation->loops()) {
        if(!loop->isSolid()) {
          loopFormation->setType(LoopFormation::DEPRESSION);
          classified = true;
        }
      }
      if(classified)
        continue;

      loopFormation->setType(LoopFormation::PROTRUSION);
    }
  }

} // namespace qr
