#include "PlaneFolder.h"

namespace qr {
  namespace {
    Transform3d transform(double xx, double xy, double xz, double yx, double yy, double yz, double zx, double zy, double zz) {
      Transform3d result;
      result.matrix() = Matrix4d::Zero();
      result(0, 0) = xx;
      result(0, 1) = xy;
      result(0, 2) = xz;
      result(1, 0) = yx;
      result(1, 1) = yy;
      result(1, 2) = yz;
      result(2, 0) = zx;
      result(2, 1) = zy;
      result(2, 2) = zz;
      result(3, 3) = 1;
      return result;
    }

    /* Creates transform src -> dst. */
    Transform3d estimateTransform(const boost::array<Vector3d, 4>& src, const boost::array<Vector3d, 4>& dst) {
      Eigen::Matrix<double, 12, 12> a = Eigen::Matrix<double, 12, 12>::Zero();
      Eigen::Matrix<double, 12, 1> b;
      for(int i = 0; i < 4; i++) {
        double x0 = dst[i].x();
        double y0 = dst[i].y();
        double z0 = dst[i].z();
        double x1 = src[i].x();
        double y1 = src[i].y();
        double z1 = src[i].z();
        int k = 3 * i;
        int l = 3 * i + 1;
        int m = 3 * i + 2;

        a(k, 0) = a(l, 4) = a(m,  8) = x1;
        a(k, 1) = a(l, 5) = a(m,  9) = y1;
        a(k, 2) = a(l, 6) = a(m, 10) = z1;
        a(k, 3) = a(l, 7) = a(m, 11) = 1;

        b[k] = x0;
        b[l] = y0;
        b[m] = z0;
      }

      /* Solve it. */
      Eigen::Matrix<double, 12, 1> f;
      a.lu().solve(b, &f);

      /* Wrap the solution into 3d transformation. */
      Eigen::Transform3d result;
      result(0, 0) = f[0];
      result(0, 1) = f[1];
      result(0, 2) = f[2];
      result(0, 3) = f[3];
      result(1, 0) = f[4];
      result(1, 1) = f[5];
      result(1, 2) = f[6];
      result(1, 3) = f[7];
      result(2, 0) = f[8];
      result(2, 1) = f[9];
      result(2, 2) = f[10];
      result(2, 3) = f[11];
      result(3, 0) = 0.0;
      result(3, 1) = 0.0;
      result(3, 2) = 0.0;
      result(3, 3) = 1.0;
      return result;
    }

  } // namespace

// -------------------------------------------------------------------------- //
// PlaneFolder
// -------------------------------------------------------------------------- //
  ViewBox* PlaneFolder::operator() () {
    View* root = NULL;

    assert(mViews.size() >= 2); 
    /* TODO: 
     * If there are only two views in the drawing, select the upper/left view as the root. */

    /* Select non-sectional view with the biggest number of adjacent views as root. */
    int maxAdjacentCount = std::numeric_limits<int>::min();
    foreach(View* view, mViews) {
      if(view->type() == View::SECTIONAL)
        continue;
      int adjacentCount = view->adjacentViews().size();
      if(adjacentCount > maxAdjacentCount) {
        maxAdjacentCount = adjacentCount;
        root = view;
      }
    }
    assert(root != NULL);

    
    /* XZ plane, positive quarter, viewspace-relative Z points outwards (-Y in our case). */
    root->setTransform(transform(1, 0, 0, 0, 0, -1, 0, 1, 0) * Translation3d(-root->boundingRect().min(0), -root->boundingRect().min(1), 0));

    root->setProjectionPlane(View::FRONT);
    fold(root, NULL);

    ViewBox* viewBox = new ViewBox();
    foreach(View* view, mViews)
      viewBox->addView(view);
    return viewBox;
  }

  void PlaneFolder::fold(View* view, View* parent) {
    if(parent != NULL) {
      /* Classify relations on direction basis. */
      boost::array<QList<ViewRelation*>, ViewRelation::MAX_DIRECTION + 1> relations;
      foreach(ViewRelation* relation, view->relations(parent))
        relations[relation->direction()].push_back(relation);
      foreach(ViewRelation* relation, parent->relations(view))
        relations[relation->direction()].push_back(relation);
      
      /* Init direction weights. */
      boost::array<double, ViewRelation::MAX_DIRECTION + 1> weights;
      for(int i = 0; i <= ViewRelation::MAX_DIRECTION; i++)
        weights[i] = 0.0;

      /* Calculate direction weights. */
      foreach(const QList<ViewRelation*>& axisRelations, relations)
        foreach(ViewRelation* relation, axisRelations)
          weights[relation->direction()] += relation->belief();

      ViewRelation::Direction dir = weights[ViewRelation::X] > weights[ViewRelation::Y] ? ViewRelation::X : ViewRelation::Y;

      assert(std::abs(weights[ViewRelation::X] - weights[ViewRelation::Y]) > 1.0e-6); /* This is not a valid assert, but it will help us catch unpleasant cases. */

      /* TODO: handle perpendicular size correspondence correctly. */

      /* Create three-point correspondence. */
      boost::array<Vector3d, 4> c, p; /* Child & parent vectors. */

      Rect2d cRect = view->boundingRect();
      Vector3d c00 = Vector3d(cRect.min(0), cRect.min(1), 0);
      Vector3d c10 = Vector3d(cRect.max(0), cRect.min(1), 0);
      Vector3d c11 = Vector3d(cRect.max(0), cRect.max(1), 0);
      Vector3d c01 = Vector3d(cRect.min(0), cRect.max(1), 0);

      Rect2d pRect = parent->boundingRect();
      Vector3d p00 = parent->transform() * Vector3d(pRect.min(0), pRect.min(1), 0);
      Vector3d p10 = parent->transform() * Vector3d(pRect.max(0), pRect.min(1), 0);
      Vector3d p11 = parent->transform() * Vector3d(pRect.max(0), pRect.max(1), 0);
      Vector3d p01 = parent->transform() * Vector3d(pRect.min(0), pRect.max(1), 0);

      Vector3d parentToChildDir;

      if(dir == ViewRelation::X) {
        if(view->center(1) > parent->center(1)) {
          c[0] = c01; p[0] = p00;
          c[1] = c11; p[1] = p10;
          parentToChildDir = Vector3d(0.0, 1.0, 0.0);
        } else {
          c[0] = c00; p[0] = p01;
          c[1] = c10; p[1] = p11;
          parentToChildDir = Vector3d(0.0, -1.0, 0.0);
        }
      } else {
        if(view->center(0) > parent->center(0)) {
          c[0] = c10; p[0] = p00;
          c[1] = c11; p[1] = p01;
          parentToChildDir = Vector3d(1.0, 0.0, 0.0);
        } else {
          c[0] = c00; p[0] = p10;
          c[1] = c01; p[1] = p11;
          parentToChildDir = Vector3d(-1.0, 0.0, 0.0);
        }
      }

      c[2] = c[0] + Vector3d(0, 0, 1); 
      p[2] = p[0] - parent->transform().linear() * parentToChildDir;

      c[3] = c[0] + parentToChildDir; 
      p[3] = p[0] + parent->transform().linear() * Vector3d(0, 0, 1);

      /* Estimate transformation. */
      Transform3d transform = estimateTransform(c, p);
      view->setTransform(transform);

      /* Get projection plane from Z direction in object space. */
      Vector3d z = transform.linear() * Vector3d(0, 0, 1);
      View::ProjectionPlane projectionPlane;
      if(z.x() < -0.1)
        projectionPlane = View::LEFT;
      else if(z.x() > 0.1)
        projectionPlane = View::RIGHT;
      else if(z.y() < -0.1)
        projectionPlane = View::REAR;
      else if(z.y() > 0.1)
        projectionPlane = View::FRONT;
      else if(z.z() < -0.1)
        projectionPlane = View::BOTTOM;
      else if(z.z() > 0.1)
        projectionPlane = View::TOP;
      else
        Unreachable();
      view->setProjectionPlane(projectionPlane);
    }
    foreach(View* child, view->adjacentViews())
      if(child != parent)
        fold(child, view);
  }

} // namespace qr
