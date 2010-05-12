#ifndef __QR_LINE_PRIMITIVE_H__
#define __QR_LINE_PRIMITIVE_H__

#include "config.h"
#include <boost/noncopyable.hpp>
#include <boost/mpl/integral_c.hpp>
#include <boost/array.hpp>
#include <QPen>
#include "Primitive.h"
#include "Algebra.h"
#include "GRect.h"
#include "GSegment.h"
#include "Utility.h"


namespace qr {
  class Hatch;
  class Vertex;
  class Loop;

// -------------------------------------------------------------------------- //
// Edge
// -------------------------------------------------------------------------- //
  class Edge: public Primitive, private boost::noncopyable {
  public:

    struct ArcData {
    public:
      ArcData(const Vector2d& center, const Vector2d& longAxis, const Vector2d& shortAxis, double startAngle, double spanAngle): 
        mCenter(center), mLongAxis(longAxis), mShortAxis(shortAxis), mStartAngle(startAngle), mSpanAngle(spanAngle) 
      {
        assert(std::abs(mLongAxis.dot(mShortAxis)) <= 1.0e-6) /* TODO: EPS */;

        /* Normalize angles. */
        if(mSpanAngle < 0) {
          mStartAngle += mSpanAngle;
          mSpanAngle = -mSpanAngle;
        }
        while(mStartAngle > 2 * M_PI)
          mStartAngle -= 2 * M_PI;
        while(mStartAngle < 0)
          mStartAngle += 2 * M_PI;
      }

      const Vector2d& center() const {
        return mCenter;
      }

      const Vector2d& longAxis() const {
        return mLongAxis;
      }

      const Vector2d& shortAxis() const {
        return mShortAxis;
      }

      Vector2d point(double pos) const {
        double angle = mStartAngle + pos * mSpanAngle;
        return mCenter + mLongAxis * cos(angle) + mShortAxis * sin(angle);
      }
      
      double startAngle() const {
        return mStartAngle;
      }

      double endAngle() const {
        return mStartAngle + mSpanAngle;
      }

      double spanAngle() const {
        return mSpanAngle;
      }

      EIGEN_MAKE_ALIGNED_OPERATOR_NEW;

    private:
      Vector2d mCenter;
      Vector2d mLongAxis, mShortAxis;
      double mStartAngle, mSpanAngle;
    };

    enum Type {
      LINE,
      ARC
    };

    enum Role {
      NORMAL, /**< Visible border in a drawing. */
      CENTER, /**< Central line. */
      PHANTOM, /**< Invisible border. */
      CUTTING, /**< Cutting line. */
      MAX_ROLE = CUTTING
    };

    typedef boost::mpl::integral_c<Type, LINE> Line;
    typedef boost::mpl::integral_c<Type, ARC> Arc;

    Edge(const Line&, const Vector2d& end0, const Vector2d& end1, const QColor& color, const Qt::PenStyle style) {
      init(end0, end1, QPen(QBrush(color), 0, style), LINE, NORMAL);
    }

    Edge(const Arc&, const Vector2d& center, const Vector2d& longAxis, const Vector2d& shortAxis, double startAngle, double spanAngle, const QColor& color, const Qt::PenStyle style) {
      ArcData& data = reinterpret_cast<ArcData&>(mData);
      new (&data) ArcData(center, longAxis, shortAxis, startAngle, spanAngle); /* This normalizes angles. */
      init(
        center + longAxis * cos(data.startAngle()) + shortAxis * sin(data.startAngle()), 
        center + longAxis * cos(data.endAngle()) + shortAxis * sin(data.endAngle()), 
        QPen(QBrush(color), 0, style), 
        ARC,
        NORMAL
      );
    }

    ~Edge() {
      switch(mType) {
      case LINE:
        break;
      case ARC:
        reinterpret_cast<ArcData&>(mData).~ArcData();
        break;
      default:
        Unreachable();
      }
    }

    template<int endIndex>
    const Vector2d& end() const {
      return mSegment.end<endIndex>();
    }

    const Vector2d& end(int endIndex) const {
      return mSegment.end(endIndex);
    }

    const Vector2d& otherEnd(const Vector2d& myEnd, double prec) const {
      return end((end(0) - myEnd).isZero(prec) ? 1 : 0);
    }

    const Vector2d& otherEnd(Edge* other, double prec) const {
      return end(isExtension(0, other, prec) ? 1 : 0);
    }

    template<int endIndex>
    Vertex* vertex() const {
      return vertex(endIndex);
    }

    Vertex* vertex(int endIndex) const {
      return mVertices[endIndex];
    }

    bool hasVertex(Vertex* vertex) const {
      return mVertices[0] == vertex || mVertices[1] == vertex;
    }

    Vertex* otherVertex(Vertex* vertex) const {
      assert(vertex == mVertices[0] || vertex == mVertices[1]);

      return mVertices[0] == vertex ? mVertices[1] : mVertices[0];
    }

    Vertex* commonVertex(Edge* other) const {
      if(mVertices[0] == other->mVertices[0] || mVertices[0] == other->mVertices[1])
        return mVertices[0];
      else if(mVertices[1] == other->mVertices[0] || mVertices[1] == other->mVertices[1])
        return mVertices[1];
      else
        return NULL;
    }

    template<int endIndex>
    void setVertex(Vertex* vertex) {
      setVertex(endIndex, vertex);
    }

    void setVertex(int endIndex, Vertex* vertex) {
      assert(vertex != NULL && mVertices[endIndex] == NULL);

      mVertices[endIndex] = vertex;
    }

    template<int endIndex>
    Vector2d tangent() const {
      return tangent(endIndex);
    }

    Vector2d tangent(double pos) const {
      if(mType == LINE) {
        return mSegment.asLine().direction().normalized();
      } else if(mType == ARC) {
        const ArcData& arc = asArc();
        return (arc.point(pos + 0.000001) - arc.point(pos - 0.000001)).normalized();
      } else {
        Unreachable();
      }
    }

    Vector2d tangent(Vertex* vertex) const {
      assert(hasVertex(vertex));

      return tangent(vertex == mVertices[0] ? 0.0 : 1.0);
    }

    /*const Vector2d& touchingEnd(Edge* other, double prec) const {
      return end(isExtension(0, other, prec) ? 0 : 1);
    }*/

    Type type() const {
      return mType;
    }

    Role role() const {
      return mRole;
    }

    void setRole(Role role) {
      mRole = role;
    }

    QColor color() const {
      return mPen.color();
    }

    Qt::PenStyle style() const {
      return mPen.style();
    }

    const QPen& pen() const {
      return mPen;
    }

    void setPen(const QPen& pen) {
      mPen = pen;
    }

    const ArcData& asArc() const {
      assert(mType == ARC);
      return reinterpret_cast<const ArcData&>(mData);
    }

    Segment2d asSegment() const {
      return mSegment;
    }
  
    QList<Edge*> extensions() const {
      QList<Edge*> result;
      result.append(mExtensions[0]);
      result.append(mExtensions[1]);
      return result;
    }

    template<int endIndex>
    const QList<Edge*>& extensions() const {
      return mExtensions[endIndex];
    }

    const QList<Edge*>& extensions(int endIndex) const {
      return mExtensions[endIndex];
    }

    const QList<Edge*>& extensions(const Vector2d& myEnd, double prec) const {
      return extensions((end(0) - myEnd).isZero(prec) ? 0 : 1);
    }

    template<int endIndex>
    void addExtension(Edge* other) {
      addExtension(endIndex, other);
    }

    void addExtension(int endIndex, Edge* other) {
      assert(isExtension(endIndex, other, 1.0e-6)); /* TODO: EPS */

      mExtensions[endIndex].push_back(other);
    }

    void addExtension(Edge* other, double prec) {
      assert(isExtension(other, prec));

      if(isExtension<0>(other, prec))
        addExtension<0>(other);
      else
        addExtension<1>(other);
    }

    Rect2d boundingRect() const {
      /* TODO for arcs? */
      return mSegment.boundingRect();
    }

    inline Rect3d boundingRect3d() const;

    Hatch* hatch() const {
      return mHatch;
    }

    void setHatch(Hatch* hatch) {
      assert(mHatch == NULL);

      mHatch = hatch;
    }

    bool isCoincident(Edge* other, double prec) {
      if(mType != other->mType)
        return false;

      switch(mType) {
      case LINE:
        return mSegment.isCoincident(other->mSegment, prec);
      case ARC: {
        return 
          (asArc().center() - other->asArc().center()).isZero(prec) &&
          (end(0) - other->end(0)).isZero(prec) && 
          (end(1) - other->end(1)).isZero(prec); /* This is correct since angles are normalized at construction time. */
      }
      default:
        Unreachable();
      }
    }

    template<int endIndex>
    bool isExtension(Edge* other, double prec) const {
      return isExtension(endIndex, other, prec);
    }

    bool isExtension(int endIndex, Edge* other, double prec) const {
      return 
        (end(endIndex) - other->end(0)).isZero(prec) || 
        (end(endIndex) - other->end(1)).isZero(prec);
    }

    bool isExtension(Edge* other, double prec) const {
      return isExtension<0>(other, prec) || isExtension<1>(other, prec);
    }

    const QList<Loop*>& loops() const {
      return mLoops;
    }

    void addLoop(Loop* loop) {
      assert(!mLoops.contains(loop));

      mLoops.push_back(loop);
    }

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW;

  protected:
    void init(const Vector2d& end0, const Vector2d& end1, const QPen& pen, Type type, Role role) {
      mSegment = Segment2d(end0, end1);
      mPen = pen;
      mType = type;
      mRole = role;
      mHatch = NULL;
      mVertices[0] = mVertices[1] = NULL;
    }

    Segment2d mSegment;
    QPen mPen;
    Type mType;
    Role mRole;
    boost::array<Vector2d, (sizeof(ArcData) + sizeof(Vector2d) - 1) / sizeof(Vector2d)> mData; /* Vector2d enforces alignment. */
    Hatch* mHatch; /* TODO: two hatches? */
    boost::array<QList<Edge*>, 2> mExtensions;
    boost::array<Vertex*, 2> mVertices;
    QList<Loop*> mLoops;
  };

} // namespace qr

#include "Vertex.h"

#endif // __QR_LINE_PRIMITIVE_H__
