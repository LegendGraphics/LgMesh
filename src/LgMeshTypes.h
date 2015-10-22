#ifndef LgMeshTypes_H
#define LgMeshTypes_H

#include <Eigen\Eigen>

namespace LgMesh {

// default use float
#ifdef LGMESH_SCALAR_TYPE
  typedef LGMESH_SCALAR_TYPE Scalar
#else
  typedef float Scalar;
#endif

typedef Eigen::Matrix<Scalar, 2, 1> Vec2;
typedef Eigen::Matrix<Scalar, 3, 1> Vec3;
typedef Vec3 Point;
typedef Vec3 Normal;
typedef Vec3 Color;
typedef Vec2 Texutre_Coordinate;

}

#endif // !LgMeshTypes_H
