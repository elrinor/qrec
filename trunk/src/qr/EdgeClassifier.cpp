#include "EdgeClassifier.h"

namespace qr {
// -------------------------------------------------------------------------- //
// EdgeClassifier
// -------------------------------------------------------------------------- //
  void EdgeClassifier::operator() () {
    foreach(Edge* edge, mDrawing->edges()) {
      switch(edge->pen().style()) {
        case Qt::DashDotLine:
          edge->setRole(Edge::CENTER);
          break;
        case Qt::DashLine:
          edge->setRole(Edge::PHANTOM);
          break;
        case Qt::SolidLine:
          if(edge->pen().color() == QColor(Qt::blue))
            edge->setRole(Edge::CUTTING);
          else
            edge->setRole(Edge::NORMAL);
          break;
        default:
          Unreachable(); /* TODO: throw something here. */
      }
    }
  }

} // namespace qr

