#include "LoopFormationConstructor.h"
#include <set>
#include <boost/foreach.hpp>
#include "LoopMerger.h"
#include "LoopUtils.h"

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

    Vector3d to3d(const Vector2d& v) {
      return Vector3d(v.x(), v.y(), 0.0);
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

      /* Evil checking for cones. */
      if(loopFormation->loops().size() >= 2) {

        Loop *circle = NULL, *trapezoid = NULL;

        foreach(Loop* loop, loopFormation->loops())
          if(LoopUtils::isCircle(loop, mPrec))
            circle = loop;

        foreach(Loop* loop, loopFormation->loops())
          if(LoopUtils::isTrapezoid(loop, mPrec))
            trapezoid = loop;

        /* TODO: >2 case not handled... */

        if(circle != NULL && trapezoid != NULL) {
          Vector2d center = circle->edge(0)->asArc().center();
          foreach(Loop* loop, circle->view()->loops()) {
            if(loop != circle && LoopUtils::isCircle(loop, mPrec) && (loop->edge(0)->asArc().center() - center).isZero(mPrec)) {
              int commonAxis = 3 - circle->view()->perpendicularAxisIndex() - trapezoid->view()->perpendicularAxisIndex();
              
              Edge* edgeOne = NULL, *edgeTwo = NULL;

              Rect1d circleOne = circle->boundingRect3d().project(commonAxis);
              Rect1d circleTwo = loop->boundingRect3d().project(commonAxis);

              Rect1d edge0 = trapezoid->edge(0)->boundingRect3d().project(commonAxis);
              Rect1d edge1 = trapezoid->edge(1)->boundingRect3d().project(commonAxis);
              Rect1d edge2 = trapezoid->edge(2)->boundingRect3d().project(commonAxis);
              Rect1d edge3 = trapezoid->edge(3)->boundingRect3d().project(commonAxis);

              if(circleOne.isCoincident(edge0, mPrec) && circleTwo.isCoincident(edge2, mPrec)) {
                edgeOne = trapezoid->edge(0);
                edgeTwo = trapezoid->edge(2);
              } else if(circleOne.isCoincident(edge2, mPrec) && circleTwo.isCoincident(edge0, mPrec)) {
                edgeOne = trapezoid->edge(2);
                edgeTwo = trapezoid->edge(0);
              } else if(circleOne.isCoincident(edge1, mPrec) && circleTwo.isCoincident(edge3, mPrec)) {
                edgeOne = trapezoid->edge(1);
                edgeTwo = trapezoid->edge(3);
              } else if(circleOne.isCoincident(edge3, mPrec) && circleTwo.isCoincident(edge1, mPrec)) {
                edgeOne = trapezoid->edge(3);
                edgeTwo = trapezoid->edge(1);
              }

              if(edgeOne != NULL && edgeTwo != NULL) {
                /* Ignore non-cone set. */
                std::set<Loop*> loopSet0;
                loopSet0.insert(circle);
                loopSet0.insert(trapezoid);
                formationSet.insert(loopSet0);
                std::set<Loop*> loopSet1;
                loopSet0.insert(loop);
                loopSet0.insert(trapezoid);
                formationSet.insert(loopSet1);

                /* Mark as cone. */
                loopFormation->addLoop(loop);
                loopFormation->setClass(LoopFormation::CONE);

                LoopFormation::Cone& cone = loopFormation->asCone();
                cone.base = circle->view()->transform() * to3d(center);
                cone.base[circle->view()->perpendicularAxisIndex()] = edgeOne->boundingRect3d().min(circle->view()->perpendicularAxisIndex());
                cone.baseX = circle->view()->transform().linear() * to3d(Vector2d(0.0, 1.0) * circle->edge(0)->asArc().longAxis().norm());
                cone.baseY = circle->view()->transform().linear() * to3d(Vector2d(1.0, 0.0) * circle->edge(0)->asArc().longAxis().norm());
                cone.topX = loop->view()->transform().linear() * to3d(Vector2d(0.0, 1.0) * loop->edge(0)->asArc().longAxis().norm());
                cone.topY = loop->view()->transform().linear() * to3d(Vector2d(1.0, 0.0) * loop->edge(0)->asArc().longAxis().norm());
                cone.height = trapezoid->view()->transform().linear() * to3d(edgeTwo->boundingRect().center() - edgeOne->boundingRect().center());
                break;
              }
            }
          }
        }
      }


      bool hasDisjoint = false;
      foreach(Loop* loop, loopFormation->loops())
        if(loop->isDisjoint())
          hasDisjoint = true;
      if(!hasDisjoint && loopFormation->loops().size() < 3 && mViewBox->views().size() > 2) {
        delete loopFormation;
        continue;
      }

      if(loopFormation->clazz() != LoopFormation::CONE) {
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

      /* If it has a hatched neighbour then it's depression. */
      foreach(Loop* loop, loopFormation->loops()) {
        if(loop->view()->type() == View::SECTIONAL && loop->isFundamental()) {
          foreach(Edge* edge, loop->edges()) {
            if(edge->hatch() != NULL) {
              loopFormation->setType(LoopFormation::DEPRESSION);
              classified = true;
              break;
            }
          }
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
