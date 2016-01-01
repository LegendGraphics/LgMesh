#include "IO.h"
#include "PolygonMesh.h"

namespace LG {



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

bool write_mesh(const PolygonMesh& mesh, const std::string& filename)
{
  // extract file extension
  std::string::size_type dot(filename.rfind("."));
  if (dot == std::string::npos) return false;
  std::string ext = filename.substr(dot+1, filename.length()-dot-1);
  std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);


  // extension determines reader
  if(ext=="obj")
  {
    return write_obj(mesh, filename);
  }

  // we didn't find a writer module
  return false;
}

}