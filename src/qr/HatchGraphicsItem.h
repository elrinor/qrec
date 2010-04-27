#ifndef __QR_HATCH_GRAPHICS_ITEM_H__
#define __QR_HATCH_GRAPHICS_ITEM_H__

#include "config.h"
#include <algorithm> /* for std::swap() */
#include <cassert>
#include "PrimitiveGraphicsItem.h"
#include "Hatch.h"
#include "Interop.h"

namespace qr {
// -------------------------------------------------------------------------- //
// HatchGraphicsItem
// -------------------------------------------------------------------------- //
  class HatchGraphicsItem: public PrimitiveGraphicsItem {
  public:
    HatchGraphicsItem(Hatch* hatch, double prec = 1.0e-6): PrimitiveGraphicsItem(hatch), mHatch(hatch), mPrec(prec) {
      assert(!hatch->segments().empty());

      if(hatch->segments().size() == 1) {
        mPainterPath.moveTo(toQPointF(hatch->segment(0)->end(0)));
        processSegment(hatch->segment(0), hatch->segment(0)->end(0));
        mPainterPath.closeSubpath();
      } else {
        Vector2d currentPosition = hatch->segment(0)->otherEnd(hatch->segment(1), mPrec);
        mPainterPath.moveTo(toQPointF(currentPosition));
        foreach(Edge* segment, hatch->segments()) {
          processSegment(segment, currentPosition);
          currentPosition = segment->otherEnd(currentPosition, mPrec);
        }
        mPainterPath.closeSubpath();
      }

      setZValue(2);
    }

    virtual QRectF boundingRect() const {
      Rect2d rect = mHatch->boundingRect();
      return QRectF(rect.min(0), -rect.max(1), rect.size(0), rect.size(1));
    }

    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* /*option*/, QWidget* /*widget*/) {
      QTransform oldTransform = painter->transform();
      painter->setTransform(QTransform(1, 0, 0, -1, 0, 0), true);
      painter->fillPath(mPainterPath, mHatch->brush());
      painter->setTransform(oldTransform);
    }

  private:
    void processSegment(const Edge* segment, const Vector2d& currectPosition) {
      switch(segment->type()) {
      case Edge::LINE:
        mPainterPath.lineTo(toQPointF(segment->otherEnd(currectPosition, mPrec)));
        break;
      case Edge::ARC: {
        const Edge::ArcData& arc = segment->asArc();
        if(
          (std::abs(arc.longAxis().x()) < mPrec && std::abs(arc.shortAxis().y()) < mPrec) ||
          (std::abs(arc.longAxis().y()) < mPrec && std::abs(arc.shortAxis().x()) < mPrec)
        ) {
          double startAngle = arc.startAngle(), spanAngle = arc.spanAngle();
          Vector2d yAxis = arc.longAxis(), xAxis = arc.shortAxis();

          if(std::abs(yAxis.y()) < mPrec) {
            using std::swap; /* To enable ADL. */
            swap(xAxis, yAxis);
            startAngle = M_PI / 2 - startAngle;
            spanAngle = -spanAngle;
          }
          if(xAxis.x() < 0) {
            xAxis = -xAxis;
            startAngle = M_PI - startAngle;
            spanAngle = -spanAngle;
          }
          if(yAxis.y() < 0) {
            yAxis = -yAxis;
            startAngle = -startAngle;
            spanAngle = -spanAngle;
          }

          if(!(currectPosition - segment->end(0)).isZero(mPrec)) {
            startAngle += spanAngle;
            spanAngle = -spanAngle;
          }

          mPainterPath.arcTo(QRectF(toQPointF(arc.center() - xAxis - yAxis), toQPointF(arc.center() + xAxis + yAxis)), startAngle, spanAngle);
        } else {
          assert(!"NOT IMPLEMENTED");
        }

        break;
      }
      default:
        Unreachable();
      }
    }

    double mPrec;
    Hatch* mHatch;
    QPainterPath mPainterPath;
  };

} // namespace qr

#endif // __QR_HATCH_GRAPHICS_ITEM_H__
