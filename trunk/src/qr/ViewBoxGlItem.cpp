#include "ViewBoxGlItem.h"
#include <QGLWidget>
#include "GPlane.h"

namespace qr {
  namespace {
    void glVertex(const Vector3d& v) {
      glVertex3d(v.x(), v.y(), v.z());
    }

    Vector3d to3d(const Vector2d& v) {
      return Vector3d(v.x(), v.y(), 0.0);
    }

    void drawSegment(View* view, Edge* segment, const QColor& color, const Plane3d& plane) {
      glColor3d(color.red() / 255.0, color.green() / 255.0, color.blue() / 255.0);

      switch(segment->style()) {
      case Qt::DashDotLine:
        glLineStipple(2, 0x3939);
        break;
      case Qt::DashLine:
        glLineStipple(2, 0x3333);
        break;
      default:
        glLineStipple(2, 0xFFFF);
        break;
      }

      glBegin(GL_LINES);
      if(segment->type() == Edge::LINE) {
        glVertex(plane.project(view->transform() * to3d(segment->end(0))));
        glVertex(plane.project(view->transform() * to3d(segment->end(1))));
      } else {
        assert(segment->type() == Edge::ARC);
        const Edge::ArcData& arc = segment->asArc();
        for(int i = 0; i < 10; i++) {
          glVertex(plane.project(view->transform() * to3d(arc.point(i / 10.0))));
          glVertex(plane.project(view->transform() * to3d(arc.point((i + 1) / 10.0))));
        }
      }
      glEnd();
    }

  } // namespace

  void ViewBoxGlItem::draw() const {
    glPushMatrix();
    Rect3d boundingRect = mViewBox->boundingRect();
    glTranslated(-boundingRect.size(0) / 2, -boundingRect.size(1) / 2, -boundingRect.size(2) / 2);

    glEnable(GL_LINE_STIPPLE);

    foreach(View* view, mViewBox->views()) {
      Plane3d viewPlane = Plane3d(view->transform() * Vector3d(0, 0, 0), view->transform().linear() * Vector3d(0, 0, 1));
      foreach(Edge* segment, view->edges())
        drawSegment(view, segment, segment->color(), viewPlane);

      if(view->type() == View::SECTIONAL) { /* TODO */
        Plane3d cuttingPlane = Plane3d(view->sourceCuttingChain()->view()->transform() * to3d(view->sourceCuttingChain()->edge(0)->end(0)), view->transform().linear() * Vector3d(0, 0, 1));
        foreach(Hatch* hatch, view->hatches()) {
          foreach(Edge* segment, hatch->segments()) {
            drawSegment(view, segment, hatch->brush().color(), cuttingPlane);
          }
        }
      }
    }

    glDisable(GL_LINE_STIPPLE);
    glPopMatrix();
  }

} // namespace qr
