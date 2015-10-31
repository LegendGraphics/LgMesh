#include "IO.h"

namespace LgMesh {


bool read_obj(PolygonMesh& mesh, const std::string& filename)
{
  char   s[200];
  float  x, y, z;
  std::vector<PolygonMesh::Vertex>  vertices;
  std::vector<Vec3> all_tex_coords;   //individual texture coordinates
  std::vector<int> halfedge_tex_idx; //texture coordinates sorted for halfedges
  PolygonMesh::Halfedge_attribute <Vec3> tex_coords = mesh.halfedge_attribute<Vec3>("h:texcoord");
  bool with_tex_coord=false;

  // clear mesh
  mesh.clear();


  // open file (in ASCII mode)
  FILE* in = fopen(filename.c_str(), "r");
  if (!in) return false;


  // clear line once
  memset(&s, 0, 200);


  // parse line by line (currently only supports vertex positions & faces
  while(in && !feof(in) && fgets(s, 200, in))
  {
    // comment
    if (s[0] == '#' || isspace(s[0])) continue;

    // vertex
    else if (strncmp(s, "v ", 2) == 0)
    {
      if (sscanf(s, "v %f %f %f", &x, &y, &z))
      {
        mesh.add_vertex(Vec3(x,y,z));
      }
    }
    // normal
    else if (strncmp(s, "vn ", 3) == 0)
    {
      if (sscanf(s, "vn %f %f %f", &x, &y, &z))
      {
        // problematic as it can be either a vertex property when interpolated
        // or a halfedge property for hard edges
      }
    }

    // texture coordinate
    else if (strncmp(s, "vt ", 3) == 0)
    {
      if (sscanf(s, "vt %f %f", &x, &y))
      {
        z=1;
        all_tex_coords.push_back(Vec3(x,y,z));
      }
    }

    // face
    else if (strncmp(s, "f ", 2) == 0)
    {
      int component(0), nV(0);
      bool endOfVertex(false);
      char *p0, *p1(s+1);

      vertices.clear();
      halfedge_tex_idx.clear();

      // skip white-spaces
      while (*p1==' ') ++p1;

      while (p1)
      {
        p0 = p1;

        // overwrite next separator

        // skip '/', '\n', ' ', '\0', '\r' <-- don't forget Windows
        while (*p1!='/' && *p1!='\r' && *p1!='\n' && *p1!=' ' && *p1!='\0') ++p1;

        // detect end of vertex
        if (*p1 != '/')
        {
          endOfVertex = true;
        }

        // replace separator by '\0'
        if (*p1 != '\0')
        {
          *p1 = '\0';
          p1++; // point to next token
        }

        // detect end of line and break
        if (*p1 == '\0' || *p1 == '\n')
        {
          p1 = 0;
        }

        // read next vertex component
        if (*p0 != '\0')
        {
          switch (component)
          {
          case 0: // vertex
            {
              vertices.push_back( PolygonMesh::Vertex(atoi(p0) - 1) );
              break;
            }
          case 1: // texture coord
            {
              int idx = atoi(p0)-1;
              halfedge_tex_idx.push_back(idx);
              with_tex_coord=true;
              break;
            }
          case 2: // normal
            break;
          }
        }

        ++component;

        if (endOfVertex)
        {
          component = 0;
          nV++;
          endOfVertex = false;
        }
      }

      PolygonMesh::Face f=mesh.add_face(vertices);


      // add texture coordinates
      if(with_tex_coord)
      {
        PolygonMesh::Halfedge_around_face_circulator h_fit = mesh.halfedges(f);
        PolygonMesh::Halfedge_around_face_circulator h_end = h_fit;
        unsigned v_idx =0;
        do
        {
          tex_coords[*h_fit]=all_tex_coords.at(halfedge_tex_idx.at(v_idx));
          ++v_idx;
          ++h_fit;
        }
        while(h_fit!=h_end);
      }
    }
    // clear line
    memset(&s, 0, 200);
  }

  fclose(in);
  return true;
}


}