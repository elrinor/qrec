#ifndef __QR_LABEL_H__
#define __QR_LABEL_H__

#include "config.h"
#include <boost/noncopyable.hpp>
#include <QString>
#include <QFont>
#include <QPen>
#include "Primitive.h"
#include "Algebra.h"

namespace qr {
// -------------------------------------------------------------------------- //
// Label
// -------------------------------------------------------------------------- //
  class Label: public Primitive, private boost::noncopyable {
  public:
    /* TODO: more params. */
    Label(const Vector2d& position, const QString& text, const QFont& font, const QColor& color): mPosition(position), mText(text), mFont(font), mPen(QPen(color)) {}

    const QString& text() const {
      return mText;
    }

    const Vector2d& position() const {
      return mPosition;
    }

    const QFont& font() const {
      return mFont;
    }

    const QPen& pen() const {
      return mPen;
    }

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW;

  private:
    Vector2d mPosition;
    QString mText;
    QFont mFont;
    QPen mPen;
  };

} // namespace qr

#endif // __QR_LABEL_H__