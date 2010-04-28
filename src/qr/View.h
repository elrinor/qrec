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
#include "Vertex.h"
#include "Loop.h"

namespace qr {
  class ViewBox;

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

    View(int id): mId(id), mType(REGULAR), mIsBoundingRectValid(false), mProjectionPlane(UNKNOWN), mTransform(Transform3d::Identity()), mSourceCuttingChain(NULL), mOuterLoop(NULL), mViewBox(NULL) {}

    bool includes(Edge* segment) const {
      return mAllEdges.contains(segment); /* TODO: slow */
    }

    void add(Edge* edge) {
      assert(!mAllEdges.contains(edge));

      mEdgesByRole[edge->role()].push_back(edge);
      mAllEdges.insert(mAllEdges.end(), edge);
      edge->setView(this);

      Vertex *end0 = vertex(edge->end(0), 1.0e-6), *end1 = vertex(edge->end(1), 1.0e-6); /* TODO: EPS */
      edge->setVertex(0, end0);
      edge->setVertex(1, end1);
      end0->addEdge(edge);
      end1->addEdge(edge);
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
      assert(cuttingChain->edges().size() > 0);
      
      foreach(Edge* edge, cuttingChain->edges())
        add(edge);

      mCuttingChains.push_back(cuttingChain);
      cuttingChain->setView(this);
    }

    void add(Loop* loop) {
      assert(!mLoops.contains(loop));

      mLoops.push_back(loop);
      loop->setView(this);
    }

    void remove(Edge* edge) {
      assert(edge->view() == this);

      mEdgesByRole[edge->role()].removeOne(edge);
      mAllEdges.removeOne(edge);

      edge->vertex(0)->removeEdge(edge);
      edge->vertex(1)->removeEdge(edge);
      if(edge->vertex(0)->edges().size() == 0)
        remove(edge->vertex(0));
      if(edge->vertex(1)->edges().size() == 0)
        remove(edge->vertex(1));

      if(edge->role() == Edge::NORMAL)
        mIsBoundingRectValid = false;
    }

    void remove(ViewRelation* relation) {
      assert(mRelations.contains(relation));

      mRelations.removeOne(relation);
    }

    const Rect2d& boundingRect() const {
      if(!mIsBoundingRectValid) {
        mBoundingRect = Rect2d();
        foreach(Edge* segment, mEdgesByRole[Edge::NORMAL])
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

    const QList<Vertex*>& vertices() const {
      return mVertices;
    }

    Vertex* vertex(const Vector2d& pos2d, double prec) {
      foreach(Vertex* vertex, mVertices)
        if((pos2d - vertex->pos2d()).isZero(prec))
          return vertex;

      Vertex* vertex = new Vertex(pos2d);
      add(vertex);
      return vertex;
    }

    const QList<Edge*>& edges() const {
      return mAllEdges;
    }

    const QList<Edge*>& edges(Edge::Role role) const {
      return mEdgesByRole[role];
    }

    const QList<Label*>& labels() const {
      return mLabels;
    }

    const QList<Hatch*>& hatches() const {
      return mHatches;
    }

    const QList<Loop*>& loops() const {
      return mLoops;
    }

    Loop* outerLoop() const {
      return mOuterLoop;
    }

    void setOuterLoop(Loop* loop) {
      assert(mLoops.contains(loop));
        
      mOuterLoop = loop;
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

    int perpendicularAxisIndex() const {
      assert(mProjectionPlane != UNKNOWN);

      switch(mProjectionPlane) {
      case LEFT:
      case RIGHT:
        return 0;
      case FRONT:
      case REAR:
        return 1;
      case TOP:
      case BOTTOM:
        return 2;
      default:
        Unreachable();
      }
    }

    void setProjectionPlane(ProjectionPlane projectionPlane) {
      mProjectionPlane = projectionPlane;
    }

    const Transform3d& transform() const {
      return mTransform;
    }

    void setTransform(const Transform3d& transform) {
      mTransform = transform;

      foreach(Vertex* vertex, mVertices)
        vertex->setPos3d(mTransform * Vector3d(vertex->pos2d().x(), vertex->pos2d().y(), 0));
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

    void add(Vertex* vertex) {
      mVertices.push_back(vertex);

      vertex->setView(this);
    }

    ViewBox* viewBox() const {
      return mViewBox;
    }

    void setViewBox(ViewBox* viewBox) {
      assert(mViewBox == NULL);

      mViewBox = viewBox;
    }

  protected:
    void remove(Vertex* vertex) {
      assert(mVertices.contains(vertex));

      mVertices.removeOne(vertex);
      delete vertex;
    }

  private:
    boost::array<QList<Edge*>, Edge::MAX_ROLE + 1> mEdgesByRole;
    QList<Vertex*> mVertices;
    QList<CuttingChain*> mCuttingChains;
    QList<Edge*> mAllEdges;
    QList<Label*> mLabels;
    QList<Hatch*> mHatches;
    QList<Loop*> mLoops;
    Loop* mOuterLoop;
    QList<ViewRelation*> mRelations;
    int mId;
    Type mType;
    QString mName;
    //QHash<View*, double> mAdjacentViews; /* View -> belief. */
    QList<View*> mAdjacentViews;
    ProjectionPlane mProjectionPlane;
    Transform3d mTransform;
    CuttingChain* mSourceCuttingChain;
    ViewBox* mViewBox;

    mutable bool mIsBoundingRectValid;
    mutable Rect2d mBoundingRect;
  };

} // namespace qr

#endif // __QR_VIEW_H__
