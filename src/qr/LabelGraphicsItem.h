#ifndef __QR_LABEL_GRAPHICS_ITEM_H__
#define __QR_LABEL_GRAPHICS_ITEM_H__

#include "config.h"
#include "PrimitiveGraphicsItem.h"
#include "Label.h"

namespace qr {
// -------------------------------------------------------------------------- //
// LabelGraphicsItem
// -------------------------------------------------------------------------- //
  class LabelGraphicsItem: public PrimitiveGraphicsItem {
  public:
    LabelGraphicsItem(Label* label): PrimitiveGraphicsItem(label), mLabel(label) {
      QFontMetrics fontMetrics(label->font());
      QRect boundingRect = fontMetrics.boundingRect(label->text());
      mBoundingRect = QRectF(QPointF(label->position().x(), -label->position().y()) + boundingRect.topLeft(), boundingRect.bottomRight());
    
      setZValue(10);
    }

    virtual QRectF boundingRect() const {
      return mBoundingRect;
    }

    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* /*option*/, QWidget* /*widget*/) {
      painter->setPen(mLabel->pen());
      painter->drawText(QRectF(mLabel->position().x(), -mLabel->position().y(), 1, 1), Qt::AlignLeft | Qt::AlignTop | Qt::TextSingleLine | Qt::TextDontClip, mLabel->text());
    }

  private:
    Label* mLabel;
    QRectF mBoundingRect;
  };

} // namespace qr

#endif // __QR_LABEL_GRAPHICS_ITEM_H__
