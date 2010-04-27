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

  private:
    Drawing* mDrawing;
    QGraphicsView* mGraphicsView;
    ViewGlView* mGlView;
    QGraphicsScene* mGraphicsScene;
    QTextEdit* mTextEdit;
  };
}


#endif // __QR_MAIN_WINDOW_H__
