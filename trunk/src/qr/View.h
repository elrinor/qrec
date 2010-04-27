#ifndef __QR_VIEW_H__
#define __QR_VIEW_H__

#include "config.h"
#include <boost/noncopyable.hpp>
#include <boost/array.hpp>
#include <QList>
#include <QSet>
#include "Edge.h"
#include "Label.h"
#include "Hatch.h"
#include "ViewRelation.h"
#include "CuttingChain.h"

namespace qr {
// -------------------------------------------------------------------------- //
// View
// -------------------------------------------------------------------------- //
  class View: public boost::noncopyable {
  public:
    enum Type {
      REGULAR,
      SECTIONAL
    };

    enum ProjectionPlane {
      FRONT,
      LEFT,
      TOP,
      RIGHT,
      BOTTOM,
      REAR,
      MAX_PROJECTION_PLANE = REAR,
      UNKNOWN = -1
    };

    View(int id): mId(id), mType(REGULAR), mIsBoundingRectValid(false), mProjectionPlane(UNKNOWN), mTransform(Transform3d::Identity()), mSourceCuttingChain(NULL) {}

    bool includes(Edge* segment) const {
      return mAllSegments.contains(segment); /* TODO: slow */
    }

    void add(Edge* segment) {
      mSegmentsByRole[segment->role()].push_back(segment);
      mAllSegments.insert(mAllSegments.end(), segment);
      segment->setView(this);
    }

    void add(Label* label) {
      mLabels.push_back(label);
      label->setView(this);
    }

    void add(Hatch* hatch) {
      mHatches.push_back(hatch);
      hatch->setView(this);
    }

    void add(ViewRelation* relation) {
      assert(relation->source() == this);
      assert(relation->type() != ViewRelation::NAME || relation->target()->type() == View::SECTIONAL);

      mRelations.push_back(relation);
    }

    void add(CuttingChain* cuttingChain) {
      assert(cuttingChain->segments().size() > 0);
      assert(cuttingChain->view() == NULL);
      
      foreach(Edge* segment, cuttingChain->segments())
        add(segment);

      mCuttingChains.push_back(cuttingChain);
      cuttingChain->setView(this);
    }

    void remove(Edge* segment) {
      assert(mAllSegments.contains(segment));

      mSegmentsByRole[segment->role()].removeOne(segment);
      mAllSegments.removeOne(segment);

      if(segment->role() == Edge::NORMAL)
        mIsBoundingRectValid = false;
    }

    void remove(ViewRelation* relation) {
      assert(mRelations.contains(relation));

      mRelations.removeOne(relation);
    }

    const Rect2d& boundingRect() const {
      if(!mIsBoundingRectValid) {
        mBoundingRect = Rect2d();
        foreach(Edge* segment, mSegmentsByRole[Edge::NORMAL])
          mBoundingRect.extend(segment->boundingRect());
        mIsBoundingRectValid = true;
      }
      return mBoundingRect;
    }

    Vector2d center() const {
      return boundingRect().center();
    }

    const double center(int index) const {
      return center()[index];
    }

    const QList<Edge*> segments() const {
      return mAllSegments;
    }

    const QList<Edge*> segments(Edge::Role role) const {
      return mSegmentsByRole[role];
    }

    const QList<Label*>& labels() const {
      return mLabels;
    }

    const QList<Hatch*>& hatches() const {
      return mHatches;
    }

    const QList<CuttingChain*>& cuttingChains() const {
      return mCuttingChains;
    }

    int id() const {
      return mId;
    }

    Type type() const {
      return mType;
    }

    void setType(Type type) {
      mType = type;
    }

    const QString& name() const {
      assert(mType == SECTIONAL);

      return mName;
    }

    void setName(const QString& name) {
      assert(mName.isEmpty() && !name.isEmpty() && mType == SECTIONAL);
      mName = name;
    }

    const QList<ViewRelation*>& relations() const {
      return mRelations;
    }

    QList<ViewRelation*> relations(View* adjacentView) const {
      QList<ViewRelation*> result;
      foreach(ViewRelation* relation, mRelations)
        if(adjacentView == relation->target())
          result.push_back(relation);
      return result;
    }

    QList<View*> adjacentViews() const {
      return mAdjacentViews/*.keys()*/;
    }

    /*double adjacentViewBelief(View* view) const {
      assert(mAdjacentViews.contains(view));

      return mAdjacentViews[view];
    }*/

    void addAdjacentView(View* view/*, double belief*/) {
      assert(!mAdjacentViews.contains(view));

      /*mAdjacentViews[view] = belief; */
      mAdjacentViews.push_back(view);
    }

    ProjectionPlane projectionPlane() const {
      assert(mProjectionPlane != UNKNOWN);

      return mProjectionPlane;
    }

    void setProjectionPlane(ProjectionPlane projectionPlane) {
      mProjectionPlane = projectionPlane;
    }

    const Transform3d& transform() const {
      return mTransform;
    }

    void setTransform(const Transform3d& transform) {
      mTransform = transform;
    }

    CuttingChain* sourceCuttingChain() const {
      assert(mType == SECTIONAL);
      return mSourceCuttingChain;
    }

    void setSourceCuttingChain(CuttingChain* cuttingChain) {
      assert(mType == SECTIONAL);
      mSourceCuttingChain = cuttingChain;
    }

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW;

  private:
    boost::array<QList<Edge*>, Edge::MAX_ROLE + 1> mSegmentsByRole;
    QList<CuttingChain*> mCuttingChains;
    QList<Edge*> mAllSegments;
    QList<Label*> mLabels;
    QList<Hatch*> mHatches;
    QList<ViewRelation*> mRelations;
    int mId;
    Type mType;
    QString mName;
    //QHash<View*, double> mAdjacentViews; /* View -> belief. */
    QList<View*> mAdjacentViews;
    ProjectionPlane mProjectionPlane;
    Transform3d mTransform;
    CuttingChain* mSourceCuttingChain;

    mutable bool mIsBoundingRectValid;
    mutable Rect2d mBoundingRect;
  };

} // namespace qr

#endif // __QR_VIEW_H__
