#include "PolygonMesh.h"

namespace LgMesh {


PolygonMesh::PolygonMesh()
{
  // allocate standard properties
  // same list is used in operator=() and assign()
  vconn_    = add_vertex_attribute<Vertex_connectivity>("v:connectivity");
  hconn_    = add_halfedge_attribute<Halfedge_connectivity>("h:connectivity");
  fconn_    = add_face_attribute<Face_connectivity>("f:connectivity");
  vpoint_   = add_vertex_attribute<Vec3>("v:point");
  vdeleted_ = add_vertex_attribute<bool>("v:deleted", false);
  edeleted_ = add_edge_attribute<bool>("e:deleted", false);
  fdeleted_ = add_face_attribute<bool>("f:deleted", false);

  deleted_vertices_ = deleted_edges_ = deleted_faces_ = 0;
  garbage_ = false;
}

PolygonMesh::~PolygonMesh()
{

}



PolygonMesh&
PolygonMesh::
operator=(const PolygonMesh& rhs)
{
  if (this != &rhs)
  {
    // deep copy of property containers
    vattrs_ = rhs.vattrs_;
    hattrs_ = rhs.hattrs_;
    eattrs_ = rhs.eattrs_;
    fattrs_ = rhs.fattrs_;

    // property handles contain pointers, have to be reassigned
    vconn_    = vertex_attribute<Vertex_connectivity>("v:connectivity");
    hconn_    = halfedge_attribute<Halfedge_connectivity>("h:connectivity");
    fconn_    = face_attribute<Face_connectivity>("f:connectivity");
    vdeleted_ = vertex_attribute<bool>("v:deleted");
    edeleted_ = edge_attribute<bool>("e:deleted");
    fdeleted_ = face_attribute<bool>("f:deleted");
    vpoint_   = vertex_attribute<Vec3>("v:point");

    // normals might be there, therefore use get_property
    vnormal_  = get_vertex_attribute<Vec3>("v:normal");
    fnormal_  = get_face_attribute<Vec3>("f:normal");

    // how many elements are deleted?
    deleted_vertices_ = rhs.deleted_vertices_;
    deleted_edges_    = rhs.deleted_edges_;
    deleted_faces_    = rhs.deleted_faces_;
    garbage_          = rhs.garbage_;
  }

  return *this;
}

PolygonMesh::Face
PolygonMesh::add_triangle(Vertex v0, Vertex v1, Vertex v2)
{
  add_face_vertices_.resize(3);
  add_face_vertices_[0] = v0;
  add_face_vertices_[1] = v1;
  add_face_vertices_[2] = v2;
  return add_face(add_face_vertices_);
}

PolygonMesh::Face
PolygonMesh::add_quad(Vertex v0, Vertex v1, Vertex v2, Vertex v3)
{
  add_face_vertices_.resize(4);
  add_face_vertices_[0] = v0;
  add_face_vertices_[1] = v1;
  add_face_vertices_[2] = v2;
  add_face_vertices_[3] = v3;
  return add_face(add_face_vertices_);
}

PolygonMesh::Face
PolygonMesh::add_face(const std::vector<Vertex>& vertices)
{
  const size_t n(vertices.size());
  assert (n > 2);

  Vertex        v;
  size_t        i, ii, id;
  Halfedge      inner_next, inner_prev, outer_next, outer_prev, boundary_next, boundary_prev, patch_start, patch_end;


  // use global arrays to avoid new/delete of local arrays!!!
  std::vector<Halfedge>&  halfedges    = add_face_halfedges_;
  std::vector<bool>&      is_new       = add_face_is_new_;
  std::vector<bool>&      needs_adjust = add_face_needs_adjust_;
  NextCache&              next_cache   = add_face_next_cache_;
  halfedges.clear();
  halfedges.resize(n);
  is_new.clear();
  is_new.resize(n);
  needs_adjust.clear();
  needs_adjust.resize(n, false);
  next_cache.clear();
  next_cache.reserve(3*n);


  // test for topological errors
  for (i = 0, ii = 1; i < n; ++i, ++ii, ii %= n)
  {
    if (!is_boundary(vertices[i]))
    {
      std::cerr << "PolygonMeshT::add_face: complex vertex\n";
      return Face();
    }

    halfedges[i] = find_halfedge(vertices[i], vertices[ii]);
    is_new[i]    = !halfedges[i].is_valid();

    if (!is_new[i] && !is_boundary(halfedges[i]))
    {
      std::cerr << "PolygonMeshT::add_face: complex edge\n";
      return Face();
    }
  }

  // re-link patches if necessary
  for (i = 0, ii = 1; i < n; ++i, ++ii, ii %= n)
  {
    if (!is_new[i] && !is_new[ii])
    {
      inner_prev = halfedges[i];
      inner_next = halfedges[ii];

      if (next_halfedge(inner_prev) != inner_next)
      {
        // here comes the ugly part... we have to relink a whole patch

        // search a free gap
        // free gap will be between boundary_prev and boundary_next
        outer_prev = opposite_halfedge(inner_next);
        outer_next = opposite_halfedge(inner_prev);
        boundary_prev = outer_prev;
        do
        {
          boundary_prev = opposite_halfedge(next_halfedge(boundary_prev));
        } while (!is_boundary(boundary_prev) || boundary_prev==inner_prev);
        boundary_next = next_halfedge(boundary_prev);
        assert(is_boundary(boundary_prev));
        assert(is_boundary(boundary_next));

        // ok ?
        if (boundary_next == inner_next)
        {
          std::cerr << "PolygonMeshT::add_face: patch re-linking failed\n";
          return Face();
        }

        // other halfedges' handles
        patch_start = next_halfedge(inner_prev);
        patch_end   = prev_halfedge(inner_next);

        // relink
        next_cache.push_back(NextCacheEntry(boundary_prev, patch_start));
        next_cache.push_back(NextCacheEntry(patch_end, boundary_next));
        next_cache.push_back(NextCacheEntry(inner_prev, inner_next));
      }
    }
  }

  // create missing edges
  for (i=0, ii=1; i<n; ++i, ++ii, ii%=n)
  {
    if (is_new[i])
    {
      halfedges[i] = new_edge(vertices[i], vertices[ii]);
    }
  }

  // create the face
  Face f(new_face());
  set_halfedge(f, halfedges[n-1]);

  // setup halfedges
  for (i=0, ii=1; i<n; ++i, ++ii, ii%=n)
  {
    v          = vertices[ii];
    inner_prev = halfedges[i];
    inner_next = halfedges[ii];

    id = 0;
    if (is_new[i])  id |= 1;
    if (is_new[ii]) id |= 2;

    if (id)
    {
      outer_prev = opposite_halfedge(inner_next);
      outer_next = opposite_halfedge(inner_prev);

      // set outer links
      switch (id)
      {
      case 1: // prev is new, next is old
        boundary_prev = prev_halfedge(inner_next);
        next_cache.push_back(NextCacheEntry(boundary_prev, outer_next));
        set_halfedge(v, outer_next);
        break;

      case 2: // next is new, prev is old
        boundary_next = next_halfedge(inner_prev);
        next_cache.push_back(NextCacheEntry(outer_prev, boundary_next));
        set_halfedge(v, boundary_next);
        break;

      case 3: // both are new
        if (!halfedge(v).is_valid())
        {
          set_halfedge(v, outer_next);
          next_cache.push_back(NextCacheEntry(outer_prev, outer_next));
        }
        else
        {
          boundary_next = halfedge(v);
          boundary_prev = prev_halfedge(boundary_next);
          next_cache.push_back(NextCacheEntry(boundary_prev, outer_next));
          next_cache.push_back(NextCacheEntry(outer_prev, boundary_next));
        }
        break;
      }

      // set inner link
      next_cache.push_back(NextCacheEntry(inner_prev, inner_next));
    }
    else needs_adjust[ii] = (halfedge(v) == inner_next);


    // set face handle
    set_face(halfedges[i], f);
  }

  // process next halfedge cache
  NextCache::const_iterator ncIt(next_cache.begin()), ncEnd(next_cache.end());
  for (; ncIt != ncEnd; ++ncIt)
  {
    set_next_halfedge(ncIt->first, ncIt->second);
  }

  // adjust vertices' halfedge handle
  for (i=0; i<n; ++i)
  {
    if (needs_adjust[i])
    {
      adjust_outgoing_halfedge(vertices[i]);
    }
  }

  return f;
}



void PolygonMesh::clear()
{
  vattrs_.resize(0);
  hattrs_.resize(0);
  eattrs_.resize(0);
  fattrs_.resize(0);

  vattrs_.free_memory();
  hattrs_.free_memory();
  eattrs_.free_memory();
  fattrs_.free_memory();

  deleted_vertices_ = deleted_edges_ = deleted_faces_ = 0;
  garbage_ = false;
}

// free redundant memory, different from clear()
void PolygonMesh::free_memory()
{
  vattrs_.free_memory();
  hattrs_.free_memory();
  eattrs_.free_memory();
  fattrs_.free_memory();
}

void PolygonMesh::
reserve(size_t nvertices,
        size_t nedges,
        size_t nfaces)
{
  vattrs_.reserve(nvertices);
  hattrs_.reserve(2 * nedges);
  eattrs_.reserve(nedges);
  fattrs_.reserve(nfaces);
}


PolygonMesh::Halfedge
PolygonMesh::find_halfedge(Vertex start, Vertex end) const
{
  assert(is_valid(start) && is_valid(end));

  Halfedge h  = halfedge(start);
  const Halfedge hh = h;

  if (h.is_valid())
  {
    do
    {
      if (to_vertex(h) == end)
      {
        return h;
      }
      h = cw_rotated_halfedge(h);
    } while (h != hh);
  }

  return Halfedge();
}



void PolygonMesh::update_face_normals()
{
  if (!fnormal_)
  {
    fnormal_ = face_attribute<Vec3>("f:normal");
  }

  Face_iterator fit = faces_begin(), fend = faces_end();
  for (; fit != fend; ++fit)
  {
    fnormal_[*fit] = compute_face_normal(*fit);
  }
}

Vec3 PolygonMesh::compute_face_normal(Face f) const
{
  Halfedge h = halfedge(f);
  Halfedge hend = h;

  Vec3 p0 = vpoint_[to_vertex(h)];
  h = next_halfedge(h);
  Vec3 p1 = vpoint_[to_vertex(h)];
  h = next_halfedge(h);
  Vec3 p2 = vpoint_[to_vertex(h)];

  if (next_halfedge(h) == hend)
  {
    // if it is a triangle
    return (p2 - p1).cross(p0 - p1).normalized();
  }
  else
  {
    Vec3 n(0, 0, 0);
    
    hend = h;
    do 
    {
      n += (p2 - p1).cross(p0 - p1);
      h = next_halfedge(h);
      p0 = p1;
      p1 = p2;
      p2 = vpoint_[to_vertex(h)];
    } while (h != hend);

    return n.normalized();
  }
}

void PolygonMesh::update_vertex_normals()
{
  if (!vnormal_)
  {
    vnormal_ = vertex_attribute<Vec3>("v:normal");
  }

  Vertex_iterator vit = vertices_begin(), vend = vertices_end();
  for (; vit != vend; ++vit)
  {
    vnormal_[*vit] = compute_vertex_normal(*vit);
  }
}

Vec3 PolygonMesh::compute_vertex_normal(Vertex v) const
{
  Vec3 n(0, 0, 0);
  Face_around_vertex_circulator fc, fc_end;
  fc = fc_end = faces(v);

  if (fc)
  {
    do 
    {
      n += compute_face_normal(*fc);
    } while (++fc != fc_end);
    n.normalize();
  }

  return n;
}

Scalar PolygonMesh::edge_length(Edge e) const
{
  return (vpoint_[vertex(e, 0)] - vpoint_[vertex(e, 1)]).norm();
}


void PolygonMesh::adjust_outgoing_halfedge(Vertex v)
{
  Halfedge h = halfedge(v);
  const Halfedge hh = h;

  if (h.is_valid())
  {
     do 
     {
      if (is_boundary(h))
      {
        set_halfedge(v, h);
        return;
      }
      h = cw_rotated_halfedge(h);
     } while (h != hh);
  }
}


void PolygonMesh::garbage_collection()
{
  // TODO
}

}