#ifndef POLYGONMESH_IO_H
#define POLYGONMESH_IO_H

#include "PolygonMesh.h"
#include <string>

namespace LgMesh {


bool read_poly(PolygonMesh& mesh, const std::string& filename);
bool read_obj(PolygonMesh& mesh, const std::string& filename);

}

#endif