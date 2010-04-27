#ifndef __QR_PLANE_FOLDER_H__
#define __QR_PLANE_FOLDER_H__

#include "config.h"
#include <limits>
#include <QList>
#include "View.h"
#include "Utility.h"
#include "ViewBox.h"

namespace qr {
// -------------------------------------------------------------------------- //
// PlaneFolder
// -------------------------------------------------------------------------- //
  class PlaneFolder {
  public:
    PlaneFolder(const QList<View*>& views): mViews(views) {}

    ViewBox* operator() ();

  private:
    void fold(View* view, View* parent);

    QList<View*> mViews;
  };

} // namespace qr

#endif // __QR_PLANE_FOLDER_H__
