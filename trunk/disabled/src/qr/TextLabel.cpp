#include "TextLabel.h"
#include <QPainter>

namespace qr {
  void TextLabel::draw(QPainter& painter) const {
    painter.setFont(mFont);
    painter.setPen(QPen(mColor));
    painter.drawText(QPointF(mPosition.x(), mPosition.y()), mText);
  }

  Rect TextLabel::boundingRect() const {
    return Rect(mPosition, Vector2d::Constant(0)); /* TODO */
  }

} // namespace qr
