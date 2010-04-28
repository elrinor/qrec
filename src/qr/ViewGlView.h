#ifndef __VIEW_GL_VIEW_H__
#define __VIEW_GL_VIEW_H__

#include "config.h"
#include <QGLWidget>
#include "View.h"
#include "ViewGlItem.h"

namespace qr {
// -------------------------------------------------------------------------- //
// ViewGlView
// -------------------------------------------------------------------------- //
  class ViewGlView: public QGLWidget {
    Q_OBJECT
  public:
    enum ProjectionType {
      ORTHOGONAL,
      PERSPECTIVE
    };

    ViewGlView(QWidget* parent = 0);
    ~ViewGlView();

    void clear();
    void addItem(ViewGlItem* item);

    ProjectionType projectionType() {
      return mProjectionType;
    }

    void setProjectionType(ProjectionType projectionType) {
      mProjectionType = projectionType;
    }

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW;

  protected:
    virtual void initializeGL();
    virtual void paintGL();
    virtual void resizeGL(int width, int height);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);

  private:
    QPoint mLastPos;
    Transform3d mTransform;
    double mEyePos;
    ProjectionType mProjectionType;
    QList<ViewGlItem*> mViewItems;
  };

} // namespace qr

#endif // __VIEW_GL_VIEW_H__
