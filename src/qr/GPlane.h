#ifndef __QR_G_PLANE_H__
#define __QR_G_PLANE_H__

#include "config.h"
#include "Algebra.h"

namespace qr {
// -------------------------------------------------------------------------- //
// GPlane
// -------------------------------------------------------------------------- //
  /**
   * GPlane defines a (N-1)-dimensional linear manifold in N-dimensional space.
   */
  template<class Vector>
  class GPlane {
  public:
    GPlane() {}

    GPlane(const Vector& origin, const Vector& normal): mOrigin(origin), mNormal(normal.normalized()) {}

    const Vector& origin() const {
      return mOrigin;
    }

    const Vector& normal() const {
      return mNormal;
    }

    Vector project(const Vector& point) const {
      return point - mNormal * (point - mOrigin).dot(mNormal);
    }

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW;

  private:
    Vector mOrigin, mNormal;
  };

  typedef GPlane<Vector3d> Plane3d;

} // namespace qr

#endif // __QR_G_PLANE_H__
