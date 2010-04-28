#include "MainWindow.h"
#include <cmath>
#include <vector>
#include <iterator>
#include <set>
#include <map>
#include <limits>
#include <algorithm>
#include <QtGui>
#include "Drawing.h"
#include "DxfReader.h"
#include "Preprocessor.h"
#include "EdgeClassifier.h"
#include "ViewConstructor.h"
#include "LoopConstructor.h"
#include "LoopFormationExtruder.h"
#include "RelationConstructor.h"
#include "RelationFilter.h"
#include "PlaneFolder.h"
#include "VertexClassifier.h"
#include "HatchGraphicsItem.h"
#include "SegmentGraphicsItem.h"
#include "LabelGraphicsItem.h"
#include "ViewGraphicsItem.h"
#include "ViewBoxGlItem.h"
#include "ViewGlView.h"
#include "LoopExtruder.h"
#include "PolyhedronGlItem.h"


#include <carve/csg.hpp>
#include <carve/tree.hpp>

namespace qr {
// -------------------------------------------------------------------------- //
// MainWindow
// -------------------------------------------------------------------------- //
  MainWindow::MainWindow() {
    mGraphicsScene = new QGraphicsScene();

    mGraphicsView = new QGraphicsView(mGraphicsScene);
    mGraphicsView->setBackgroundBrush(QBrush(Qt::black));
    mGraphicsView->resize(500, 500);

    mGlView = new ViewGlView();

    mTextEdit = new QTextEdit;
    mTextEdit->setReadOnly(true);

    QSplitter* splitter = new QSplitter(Qt::Horizontal);
    splitter->addWidget(mGraphicsView);
    splitter->addWidget(mGlView);
    splitter->addWidget(mTextEdit);

    setCentralWidget(splitter);

    QAction* openAction = new QAction("&Open...", this);
    connect(openAction, SIGNAL(triggered()), this, SLOT(openFile()));

    QAction* closeAction = new QAction("E&xit", this);
    connect(closeAction, SIGNAL(triggered()), this, SLOT(close()));

    QAction* orthoAction = new QAction("&Orthogonal", this);
    connect(orthoAction, SIGNAL(triggered()), this, SLOT(useOrthogonal()));

    QAction* perspAction = new QAction("&Perspective", this);
    connect(perspAction, SIGNAL(triggered()), this, SLOT(usePerspective()));

    QAction* projAction = new QAction("&Projections", this);
    connect(projAction, SIGNAL(triggered()), this, SLOT(showProjections()));

    QAction* solidAction = new QAction("&Solid", this);
    connect(solidAction, SIGNAL(triggered()), this, SLOT(showSolid()));

    QMenu* fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction(openAction);
    fileMenu->addSeparator();
    fileMenu->addAction(closeAction);
    
    QMenu* viewMenu = menuBar()->addMenu("&View");
    viewMenu->addAction(orthoAction);
    viewMenu->addAction(perspAction);
    viewMenu->addSeparator();
    viewMenu->addAction(projAction);
    viewMenu->addAction(solidAction);

    mDrawing = NULL;
    mShowProjection = false;

    resize(800, 600);
  }

  void MainWindow::openFile() {
    QString targetPath = QFileDialog::getOpenFileName(this, "Choose DXF file...", ".", "DXF Files (*.dxf)"); 
    if(targetPath.isEmpty())
      return;

    if(mDrawing != NULL)
      delete mDrawing;
    mDrawing = new Drawing();

    QFile file(targetPath);
    file.open(QIODevice::ReadOnly);
    DxfReader(file, mDrawing)();

    (void) EdgeClassifier(mDrawing)();
    (void) Preprocessor(mDrawing, 1.0e-6)();
    QList<View*> views = ViewConstructor(mDrawing, 1.0e-6)();
    (void) VertexClassifier(views)();
    (void) LoopConstructor(views, 1.0e-6)();
    (void) RelationConstructor(views, 1.0e-6)();
    (void) RelationFilter(views)();
    ViewBox* viewBox = PlaneFolder(views)();

    mGraphicsScene->clear();
    QString plainText;
    foreach(View* view, views) {
      mGraphicsScene->addItem(new ViewGraphicsItem(view));
      foreach(Hatch* hatch, view->hatches())
        mGraphicsScene->addItem(new HatchGraphicsItem(hatch));
      foreach(Edge* segment, view->edges())
        mGraphicsScene->addItem(new SegmentGraphicsItem(segment));
      foreach(Label* label, view->labels())
        mGraphicsScene->addItem(new LabelGraphicsItem(label));
      foreach(ViewRelation* relation, view->relations()) {
        plainText.append(
          QString::number(relation->source()->id()) + "->" + QString::number(relation->target()->id()) + ": " + 
          (relation->direction() == ViewRelation::X ? "(X)" : "(Y)") + " " + 
          (relation->type() == ViewRelation::PARALLEL ? "parallel" : (relation->type() == ViewRelation::PERPENDICULAR ? "perpendicular" : (relation->type() == ViewRelation::NAME ? "name" : "center"))) + 
          "\n"
        );
      }
    }

    std::vector<Loop*> boundOnSolid;
    foreach(View* view, views)
      boundOnSolid.push_back(view->outerLoop());
    carve::poly::Polyhedron* poly = LoopFormationExtruder(boundOnSolid, 64)();

    mViewBoxGlItem = new ViewBoxGlItem(viewBox);
    mPolyhedronGlItem = new PolyhedronGlItem(poly);

    mGlView->clear();
    if(mShowProjection)
      mGlView->addItem(mViewBoxGlItem);
    else
      mGlView->addItem(mPolyhedronGlItem);

    mTextEdit->setPlainText(plainText);
  }

  void MainWindow::useOrthogonal() {
    mGlView->setProjectionType(ViewGlView::ORTHOGONAL);
    mGlView->updateGL();
  }

  void MainWindow::usePerspective() {
    mGlView->setProjectionType(ViewGlView::PERSPECTIVE);
    mGlView->updateGL();
  }

  void MainWindow::showProjections() {
    mShowProjection = true;
    mGlView->clear();
    mGlView->addItem(mViewBoxGlItem);
    mGlView->updateGL();
  }

  void MainWindow::showSolid() {
    mShowProjection = false;
    mGlView->clear();
    mGlView->addItem(mPolyhedronGlItem);
    mGlView->updateGL();
  }


} // namespace qr

