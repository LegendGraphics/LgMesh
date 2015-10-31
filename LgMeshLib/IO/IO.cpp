#include "IO.h"
#include "PolygonMesh.h"

namespace LgMesh {



bool read_poly(PolygonMesh& mesh, const std::string& filename)
{

  std::size_t found = filename.find_last_of('.');
  if (found == std::string::npos)
  {
    return false;
  }
  std::string ext = filename.substr(found + 1);
  std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

  if (ext == "obj")
  {
    return read_obj(mesh, filename);
  }

  return false;
}



}