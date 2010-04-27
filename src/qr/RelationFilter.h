#ifndef __QR_RELATION_FILTER_H__
#define __QR_RELATION_FILTER_H__

#include "config.h"
#include <QList>
#include "View.h"

namespace qr {
// -------------------------------------------------------------------------- //
// RelationFilter
// -------------------------------------------------------------------------- //
  class RelationFilter {
  public:
    RelationFilter(QList<View*> views): mViews(views) {}

    void operator() ();

  private:
    QList<View*> mViews;
  };

} // namespace qr

#endif // __QR_RELATION_FILTER_H__
