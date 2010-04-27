#include "config.h"
#include <QApplication>
#include "qr/MainWindow.h"

#include "qr/Algebra.h"

#include "qr/GSegment.h"

int main(int argc, char** argv) {
  QApplication app(argc, argv);

  qr::MainWindow mainWindow;
  mainWindow.show();
  return app.exec();
}