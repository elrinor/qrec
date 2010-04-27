#ifndef __QR_VIEW_GRAPHICS_ITEM_H__
#define __QR_VIEW_GRAPHICS_ITEM_H__

#include "config.h"
#include <QPainter>
#include "Algebra.h"
#include "View.h"
#include "Utility.h"

namespace qr {
// -------------------------------------------------------------------------- //
// ViewGraphicsItem
// -------------------------------------------------------------------------- //
  class ViewGraphicsItem: public QGraphicsItem {
  public:
    ViewGraphicsItem(const View* view): mView(view) {
      /* Calculate real bounding rectangle. */
      qr::Rect2d rect = view->boundingRect();
      foreach(qr::Edge* segment, view->segments())
        rect.extend(segment->boundingRect());
      foreach(qr::Label* label, view->labels()) {
        rect.extend(label->position());
        rect.extend(qr::Vector2d(label->position().x(), label->position().y() + label->font().pointSize()));
      }

      /* Convert to window coordinate system & adjust. */
      mRect = toQRectF(rect);
      mRect = QRectF(mRect.left(), -mRect.bottom(), mRect.width(), mRect.height());
      mRect.adjust(0, -10, 0, 0);

      mLabel = QString::number(view->id()) + ":" + (view->type() == View::REGULAR ? "REG" : "SEC") + ":" + projectionPlaneName(view->projectionPlane());

      setZValue(0);
    }

    virtual QRectF boundingRect() const {
      return mRect;
    }

    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* /*option*/, QWidget* /*widget*/) {
      painter->setPen(QPen(QColor(Qt::darkGray)));
      painter->drawRect(mRect);
      painter->setFont(QFont("Arial", 6));
      painter->drawText(mRect, Qt::AlignTop | Qt::AlignLeft, mLabel);
    }

  private:
    static QString projectionPlaneName(View::ProjectionPlane projectionPlane) {
      switch(projectionPlane) {
      case View::FRONT:
        return "FRONT";
      case View::REAR:
        return "REAR";
      case View::LEFT:
        return "LEFT";
      case View::RIGHT:
        return "RIGHT";
      case View::TOP:
        return "TOP";
      case View::BOTTOM:
        return "BOTTOM";
      default:
        Unreachable();
      }
    }

    const View* mView;
    QRectF mRect;
    QString mLabel;
  };

} // namespace qr


#endif // __QR_VIEW_GRAPHICS_ITEM_H__
