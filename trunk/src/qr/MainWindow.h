#ifndef __QR_MAIN_WINDOW_H__
#define __QR_MAIN_WINDOW_H__

#include "config.h"
#include <QMainWindow>
#include <QTextEdit>
#include <QGraphicsView>
#include <QGraphicsScene>

namespace qr {
  class Drawing;
  class ViewGlView;
  class ViewBoxGlItem;
  class PolyhedronGlItem;

// -------------------------------------------------------------------------- //
// MainWindow
// -------------------------------------------------------------------------- //
  class MainWindow: public QMainWindow {
    Q_OBJECT
  public:
    MainWindow();

  protected slots:
    void openFile();
    void useOrthogonal();
    void usePerspective();
    void showProjections();
    void showSolid();

  private:
    Drawing* mDrawing;
    QGraphicsView* mGraphicsView;
    ViewGlView* mGlView;
    QGraphicsScene* mGraphicsScene;
    QTextEdit* mTextEdit;

    ViewBoxGlItem* mViewBoxGlItem;
    PolyhedronGlItem* mPolyhedronGlItem;
    bool mShowProjection;
  };
}


#endif // __QR_MAIN_WINDOW_H__
