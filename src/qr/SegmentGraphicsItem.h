#ifndef __QR_SEGMENT_GRAPHICS_ITEM_H__
#define __QR_SEGMENT_GRAPHICS_ITEM_H__

#include "config.h"
#include "PrimitiveGraphicsItem.h"
#include "Edge.h"
#include "Interop.h"

namespace qr {
// -------------------------------------------------------------------------- //
// SegmentGraphicsItem
// -------------------------------------------------------------------------- //
  class SegmentGraphicsItem: public PrimitiveGraphicsItem {
  public:
    SegmentGraphicsItem(Edge* segment): PrimitiveGraphicsItem(segment), mSegment(segment) {
      if(segment->role() == Edge::CUTTING)
        setZValue(10);
      else
        setZValue(11);
    }

    virtual QRectF boundingRect() const {
      Rect2d rect = mSegment->boundingRect();
      return QRectF(rect.min(0), -rect.max(1), rect.size(0), rect.size(1));
    }

    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* /*option*/, QWidget* /*widget*/) {
      QTransform oldTransform = painter->transform();
      painter->setTransform(QTransform(1, 0, 0, -1, 0, 0), true);
      switch(mSegment->type()) {
      case Edge::LINE:
        painter->setPen(mSegment->pen());
        painter->drawLine(toQPointF(mSegment->end(0)), toQPointF(mSegment->end(1)));
        break;
      case Edge::ARC: {
        QTransform oldTransform = painter->transform();

        const Edge::ArcData& arc = mSegment->asArc();

        /* QTransform is col-major. */
        QTransform newTransform = QTransform(
          arc.longAxis().x(), 
          arc.shortAxis().x(),
          arc.longAxis().y(),
          arc.shortAxis().y(),
          arc.center().x(),
          arc.center().y()
        );
        painter->setTransform(newTransform, true);

        painter->setPen(mSegment->pen());
        painter->drawArc(
          QRect(-1, -1, 2, 2), 
          static_cast<int>(arc.startAngle() / M_PI * 180 * 16), 
          static_cast<int>(arc.spanAngle() / M_PI * 180 * 16)
        );
        painter->setTransform(oldTransform);
        break;
      }
      default:
        Unreachable();
      }
      painter->setTransform(oldTransform);
    }

  private:
    Edge* mSegment;
  };

} // namespace qr

#endif // __QR_SEGMENT_GRAPHICS_ITEM_H__
