#include "PolyhedronGlItem.h"
#include <QGLWidget>
#include "Algebra.h"

namespace qr {
  namespace {
    inline void glVertex(const Vector3d& v) {
      glVertex3d(v.x(), v.y(), v.z());
    }

    inline void glNormal(const Vector3d& v) {
      glNormal3d(v.x(), v.y(), v.z());
    }

    inline Vector3d toVector3d(const carve::poly::Vertex<3> &v) {
      return Vector3d(v.v.x, v.v.y, v.v.z);
    }

    void __stdcall faceBegin(GLenum type, void* data) {
      carve::poly::Face<3>& face = *static_cast<carve::poly::Face<3>*>(data);
      glBegin(type);
      glNormal(toVector3d(face.plane_eqn.N));
    }

    void __stdcall faceVertex(void *vertex_data, void* /*data*/) {
      carve::geom3d::Vector& vertex(*static_cast<carve::geom3d::Vector*>(vertex_data));
      glVertex3f(vertex.x, vertex.y, vertex.z);
    }

    void __stdcall faceEnd(void* /*data*/) {
      glEnd();
    }

    void drawFace(const carve::poly::Face<3>& face) {
      GLUtesselator* tess = gluNewTess();

      typedef void (__stdcall *GLUTessCallback)();
      gluTessCallback(tess, GLU_TESS_BEGIN_DATA, (GLUTessCallback) faceBegin);
      gluTessCallback(tess, GLU_TESS_VERTEX_DATA, (GLUTessCallback) faceVertex);
      gluTessCallback(tess,  GLU_TESS_END_DATA, (GLUTessCallback) faceEnd);

      gluTessBeginPolygon(tess, const_cast<GLvoid*>(static_cast<const GLvoid*>(&face)));
      gluTessBeginContour(tess);
      for(std::size_t i = 0; i < face.vertices.size(); i++)
        gluTessVertex(tess, const_cast<GLdouble*>(static_cast<const GLdouble*>(face.vertices[i]->v.v)), const_cast<GLvoid*>(static_cast<const GLvoid*>(&face.vertices[i]->v)));
      gluTessEndContour(tess);
      gluTessEndPolygon(tess);

      gluDeleteTess(tess);
    }

    void drawPolyhedron(carve::poly::Polyhedron *poly) {
      glBegin(GL_TRIANGLES);
      for(std::size_t i = 0; i < poly->faces.size(); i++) {
        carve::poly::Face<3> &face = poly->faces[i];
        if(face.vertices.size() == 3) {
          glNormal(toVector3d(face.plane_eqn.N));
          glVertex(toVector3d(*face.vertices[0]));
          glVertex(toVector3d(*face.vertices[1]));
          glVertex(toVector3d(*face.vertices[2]));
        }
      }
      glEnd();

      for(size_t i = 0, l = poly->faces.size(); i != l; ++i) {
        carve::poly::Face<3> &face = poly->faces[i];
        if(face.vertices.size() != 3)
          drawFace(poly->faces[i]);
      }
    }
  
  } // namespace

// -------------------------------------------------------------------------- //
// PolyhedronGlItem
// -------------------------------------------------------------------------- //
  PolyhedronGlItem::PolyhedronGlItem(carve::poly::Polyhedron* polyhedron): mPoly(polyhedron) {
    //polyhedron->canonicalize();
  }

  void PolyhedronGlItem::draw() {
    glPushAttrib(GL_ENABLE_BIT | GL_POLYGON_BIT);
    glEnable(GL_LIGHTING);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glPushMatrix();
    glTranslated(-mPoly->aabb.pos.x, -mPoly->aabb.pos.y, -mPoly->aabb.pos.z);

    glColor3f(1, 1, 1);
    drawPolyhedron(mPoly);

    glPopMatrix();
    glPopAttrib();
  }

} // namespace qr