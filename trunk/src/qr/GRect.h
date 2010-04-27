#ifndef __QR_G_RECT_H__
#define __QR_G_RECT_H__

#include "config.h"
#include "Algebra.h"

namespace qr {
  template<class Vector>
  class GLine;

  template<class Vector>
  class GSegment;

// -------------------------------------------------------------------------- //
// GRect
// -------------------------------------------------------------------------- //
  /**
   * GRect defines an axis-aligned bounding box in N-dimensional space.
   */
  template<class Vector>
  class GRect {
  public:
    /** Default Constructor. 
     *
     * Creates empty rectangle. */
    GRect() {
      mMin.setConstant(std::numeric_limits<double>::max());
      mMax.setConstant(-std::numeric_limits<double>::max());
    }

    /** Constructor.
     *
     * @param min                      Coordinates of the minimal corner.
     * @param size                     Size. */
    GRect(const Vector& min, const Vector& size): mMin(min), mMax(min + size) {
      assert((size.cwise() >= 0).all()); /* We allow "thin" rectangles, therefore ">=". */
    }

    GRect(double x, double y, double width, double height): mMin(x, y), mMax(x + width, y + height) {
      assert(width >= 0 && height >= 0);
    }

    /** @returns                       Empty rectangle. */
    static GRect emptyRect() {
      GRect result;
      result.mMin.setConstant(std::numeric_limits<double>::max());
      result.mMax.setConstant(-std::numeric_limits<double>::max());
      return result;
    }

    /** @returns                       Infinite rectangle containing everything. */
    static GRect infiniteRect() {
      GRect result;
      result.mMin.setConstant(-std::numeric_limits<double>::max());
      result.mMax.setConstant(std::numeric_limits<double>::max());
      return result;
    }

    const Vector& min() const {
      return mMin;
    }

    const Vector& max() const {
      return mMax;
    }

    /** @returns                       Coordinate of the minimal corner. */
    double min(int index) const {
      return mMin[index];
    }

    /** @returns                       Coordinate of the maximal corner. */
    double max(int index) const {
      return mMax[index];
    }

    void setMin(const Vector& value) {
      mMin = value;
    }

    void setMax(const Vector& value) {
      mMax = value;
    }

    void setMin(int index, double value) {
      mMin[index] = value;
    }

    void setMax(int index, double value) {
      mMax[index] = value;
    }

    bool isEmpty() const {
      return (mMin.cwise() > mMax).any();
    }

    bool isEmpty(double prec) const {
      return ((mMin.cwise() - prec).cwise()  > mMax).any();
    }

    bool isInfinite() const {
      return 
        (mMax.cwise() >=  std::numeric_limits<double>::max()).any() ||
        (mMin.cwise() <= -std::numeric_limits<double>::max()).any();
    }

    /**
     * Extends this rectangle to include given point.
     */
    void extend(const Vector& point) {
      mMin = mMin.cwise().min(point);
      mMax = mMax.cwise().max(point);
    }

    /**
     * Extends this rectangle to include other rectangle.
     */
    void extend(const GRect& other) {
      mMin = mMin.cwise().min(other.mMin);
      mMax = mMax.cwise().max(other.mMax);
    }

    void intersect(const GRect& other) {
      mMin = mMin.cwise().max(other.mMin);
      mMax = mMax.cwise().min(other.mMax);
    }

    Vector size() const {
      return mMax - mMin;
    }

    double size(int index) const {
      return mMax[index] - mMin[index];
    }

    Vector center() const {
      return (mMin + mMax) / 2;
    }

    /**
     * @returns                        true if there exists an intersection (maybe of zero size) between two rectangles.
     */
    /*bool intersects(const GRect& other) const {
      return (mMin.cwise() <= other.mMax).all() && (other.mMin.cwise() <= mMax).all();
    }*/

    /*bool contains(const GRect& other) const {
      return (mMin.cwise() <= other.mMin).all() && (other.mMax.cwise() <= mMax).all();
    }*/

    bool intersects(const GRect& other, double prec) const {
      return ((mMin.cwise() - prec).cwise() <= other.mMax).all() && ((other.mMin.cwise() - prec).cwise() <= mMax).all();
    }

    bool intersects(const GLine<Vector>& line, double prec) const;

    bool intersects(const GSegment<Vector>& segment, double prec) const;

    bool contains(const GSegment<Vector>& segment, double prec) const;

    bool contains(const GRect& other, double prec) const {
      return ((mMin.cwise() - prec).cwise() <= other.mMin).all() && ((other.mMax.cwise() - prec).cwise() <= mMax).all();
    }

    bool contains(const Vector& point, double prec) const {
      return ((mMin.cwise() - prec).cwise() <= point).all() && ((point.cwise() - prec).cwise() <= mMax).all();
    }

    bool empty() const {
      return (mMin.cwise() > mMax).any();
    }

    bool infinite() const {
      return 
        (mMax.cwise() >=  std::numeric_limits<double>::max()).any() ||
        (mMin.cwise() <= -std::numeric_limits<double>::max()).any();
    }

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW;

  private:
    Vector mMin, mMax;
  };

  typedef GRect<Vector1d> Rect1d;
  typedef GRect<Vector2d> Rect2d;
  typedef GRect<Vector3d> Rect3d;

} // namespace qr

#include "Interop.h"

#endif // __QR_G_RECT_H__
