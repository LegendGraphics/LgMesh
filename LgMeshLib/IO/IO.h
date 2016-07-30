#ifndef POLYGONMESH_IO_H
#define POLYGONMESH_IO_H

#include "PolygonMesh.h"
#include <string>

namespace LG {


bool read_poly(PolygonMesh& mesh, const std::string& filename);
bool read_obj(PolygonMesh& mesh, const std::string& filename);

bool write_poly(const PolygonMesh& mesh, const std::string& filename);
bool write_obj(const PolygonMesh& mesh, const std::string& filename);

}

#endif