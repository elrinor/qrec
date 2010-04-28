#include "ViewGlView.h"
#include <algorithm> /* for std::min() */
#include <QMouseEvent>

namespace qr {
  ViewGlView::ViewGlView(QWidget* parent): QGLWidget(parent), mProjectionType(PERSPECTIVE) {
    mEyePos = 300;
    mTransform = Matrix4d::Identity();
  }

  ViewGlView::~ViewGlView() {
    /* TODO */
  }

  void ViewGlView::clear() {
    mViewItems.clear();

    updateGL();
  }

  void ViewGlView::addItem(ViewGlItem* item) {
    mViewItems.push_back(item);
    
    updateGL();
  }

  void ViewGlView::initializeGL() {
    glClearColor(0, 0, 0, 0);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDisable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHT0);

    GLfloat lightAmb[] = {0.1f, 0.1f, 0.1f, 1.0f};
    GLfloat lightDif[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat lightSpc[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDif);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpc);
  }

  void ViewGlView::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    /* Set up projection transformation. */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    double ratio = static_cast<double>(width()) / height();
    if(mProjectionType == PERSPECTIVE)
      gluPerspective(90, ratio, 1, 1000);
    else if(mProjectionType == ORTHOGONAL)
      glOrtho(-200 * ratio, 200 * ratio, -200, 200, 1, 1000);
    else
      Unreachable();

    /* Set up modelview transformation. */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, -mEyePos, 0, 0, 0, 0, 0, 0, 1);

    /* Set light position. */
    GLfloat lightPos[] = {-mEyePos, -mEyePos, mEyePos, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    /* Rotate scene. */
    glMultMatrixd(mTransform.data());

    /* Render! */
    foreach(ViewGlItem* item, mViewItems)
      item->draw();
  }

  void ViewGlView::resizeGL(int width, int height) {
    glViewport(0, 0, width, height);
  }

  void ViewGlView::mousePressEvent(QMouseEvent* event) {
     mLastPos = event->pos();
  }

  void ViewGlView::mouseMoveEvent(QMouseEvent* event) {
    int dx = event->x() - mLastPos.x();
    int dy = event->y() - mLastPos.y();

    double sensitivity = 0.005; /* TODO */
    mTransform = AngleAxisd(sensitivity * dx, Vector3d(0, 0, 1)) * AngleAxisd(sensitivity * dy, Vector3d(1, 0, 0)) * mTransform;
    mLastPos = event->pos();

    updateGL();
  }

} // namespace qr