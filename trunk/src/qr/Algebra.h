#ifndef __QR_ALGEBRA_H__
#define __QR_ALGEBRA_H__

#include <cmath>
#include <Eigen/Dense>
#include <Eigen/Geometry>

#ifndef M_PI
#  define M_PI 3.14159265358979323846
#endif

namespace qr {
  using Eigen::Matrix;
  using Eigen::Vector2d;
  using Eigen::Vector3d;
  using Eigen::Vector4d;
  using Eigen::Matrix2d;
  using Eigen::Matrix3d;
  using Eigen::Matrix4d;

  typedef Matrix<double, 1, 1> Vector1d;
  typedef Vector1d Matrix1d;

  using Eigen::Transform3d;
  using Eigen::Translation3d;
  using Eigen::AngleAxisd;

} // namespace qr

#endif // __QR_ALGEBRA_H__
