#include "Debug.h"
#include "ViewGlView.h"
#include "PolyhedronGlItem.h"
#include <QDialog>
#include <QHBoxLayout>

#if 1

#include <boost/lexical_cast.hpp>
#include <carve/src/write_ply.hpp>
#ifdef _DEBUG
#  pragma comment(lib, "fileformatsd.lib")
#  pragma comment(lib, "gloopd.lib")
#else
#  pragma comment(lib, "fileformats.lib")
#  pragma comment(lib, "gloop.lib")
#endif

#endif


namespace qr {
  void debugShowPoly(carve::poly::Polyhedron* poly) { 
    ViewGlView* glView = new ViewGlView();
    glView->addItem(new PolyhedronGlItem(poly));

    QDialog* dlg = new QDialog();
    QHBoxLayout* layout = new QHBoxLayout();
    layout->addWidget(glView);
    dlg->setLayout(layout);
    dlg->resize(500, 500);
    dlg->exec();
  }


  void debugSavePoly(carve::poly::Polyhedron* poly, carve::csg::CSG::OP op) {
    static int index = 0;

    writePLY("_" + boost::lexical_cast<std::string>(index++) + (op == carve::csg::CSG::UNION ? "a" : op == carve::csg::CSG::A_MINUS_B ? "s" : "i") + ".ply", poly);
  }

} // namespace qr
