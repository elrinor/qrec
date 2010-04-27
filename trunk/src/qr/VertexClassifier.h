#ifndef __QR_VERTEX_CLASSIFIER_H__
#define __QR_VERTEX_CLASSIFIER_H__

#include "config.h"
#include <QList>
#include "View.h"

namespace qr {
// -------------------------------------------------------------------------- //
// VertexClassifier
// -------------------------------------------------------------------------- //
  class VertexClassifier {
  public:
    VertexClassifier(const QList<View*> views): mViews(views) {}

    void operator() ();

  private:
    QList<View*> mViews;
  };

} // namespace qr

#endif // __QR_VERTEX_CLASSIFIER_H__
