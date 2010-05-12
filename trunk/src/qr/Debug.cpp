#include "Debug.h"
#include "ViewGlView.h"
#include "PolyhedronGlItem.h"
#include <QDialog>
#include <QHBoxLayout>

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

} // namespace qr
