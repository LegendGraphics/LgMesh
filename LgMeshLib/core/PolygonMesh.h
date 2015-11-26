#ifndef PolygonMesh_H
#define PolygonMesh_H

#include "Attributes.h"
#include "Kernel.h"
#include "LgMeshTypes.h"

namespace LG {


class PolygonMesh : public Kernel
{

public: //--- topolygo types

  class BaseHandle
  {
  public:
    
    // Constructor
    explicit BaseHandle(int _idx = -1) : idx_(_idx) {}

    // Get index of this handle
    int idx() const { return idx_; };

    // Reset handle to be invalid (index = -1)
    void reset() { idx_ = -1; };

    // Return whether the handle is valid
    bool is_valid() const { return idx_ != -1; };

    // Override compare operator
    // Is equal
    bool operator == (const BaseHandle& _rhs) const
    {
      return idx_ == _rhs.idx_;
    }

    // Is different
    bool operator != (const BaseHandle& _rhs) const
    {
      return idx_ != _rhs.idx_;
    }

    // Is less than
    bool operator < (const BaseHandle& _rhs) const
    {
      return idx_ < _rhs.idx_;
    }

  private:
    friend class Vertex_iterator;
    friend class Halfedge_iterator;
    friend class Edge_iterator;
    friend class Face_iterator;
    friend class PolygonMesh;

    int idx_;
  };


  // primitive type (internally it is an index)
  // Vertex
  struct Vertex : public BaseHandle
  {
    // default constructor
    explicit Vertex(int _idx = -1) : BaseHandle(_idx) {}
    std::ostream& operator << (std::ostream& os) const { return os << 'v' << idx(); };
  };

  // Halfedge
  struct Halfedge : public BaseHandle
  {
    explicit Halfedge(int _idx = -1) : BaseHandle(_idx) {}
    std::ostream& operator << (std::ostream& os) const { return os << "he" << idx(); };
  };

  // Edge
  struct Edge : public BaseHandle
  {
    explicit Edge(int _idx = -1) : BaseHandle(_idx) {}
    std::ostream& operator << (std::ostream& os) const { return os << 'e' << idx(); };
  };

  // Face
  struct Face : public BaseHandle
  {
    explicit Face(int _idx = -1) : BaseHandle(_idx) {}
    std::ostream& operator << (std::ostream& os) const { return os << 'f' << idx(); };
  };

public: //-------------------------------------------------- connectivity types

  // This type stores the vertex connectivity
  // a Halfedge_connectivity, Face_connectivity
  struct Vertex_connectivity
  {
    /// an outgoing halfedge per vertex (it will be a boundary halfedge for boundary vertices)
    Halfedge  halfedge_;
  };


  // This type stores the halfedge connectivity
  // a Vertex_connectivity, Face_connectivity
  struct Halfedge_connectivity
  {
    /// face incident to halfedge
    Face      face_;
    /// vertex the halfedge points to
    Vertex    vertex_;
    /// next halfedge within a face (or along a boundary)
    Halfedge  next_halfedge_;
    /// previous halfedge within a face (or along a boundary)
    Halfedge  prev_halfedge_;
  };


  // This type stores the face connectivity
  // a Vertex_connectivity, Halfedge_connectivity
  struct Face_connectivity
  {
    /// a halfedge that is part of the face
    Halfedge  halfedge_;
  };


public: //--- attribute types

  template <class T>
  class Vertex_attribute : public Attribute<T>
  {
  public:

    explicit Vertex_attribute() {}
    explicit Vertex_attribute(Attribute<T> attr) : Attribute<T>(attr) {}

    // random access the data stored for vertex v
    typename Attribute<T>::reference operator[](Vertex v)
    {
      return Attribute<T>::operator[](v.idx());
    }

    typename Attribute<T>::const_reference operator[](Vertex v) const
    {
      return Attribute<T>::operator[](v.idx());
    }
  };

  template <class T>
  class Halfedge_attribute : public Attribute<T>
  {
  public:

    explicit Halfedge_attribute() {}
    explicit Halfedge_attribute(Attribute<T> attr) : Attribute<T>(attr) {}

    // random access the data stored for halfedge he
    typename Attribute<T>::reference operator[](Halfedge he)
    {
      return Attribute<T>::operator[](he.idx());
    }

    typename Attribute<T>::const_reference operator[](Halfedge he) const
    {
      return Attribute<T>::operator[](he.idx());
    }
  };

  template <class T>
  class Edge_attribute : public Attribute<T>
  {
  public:

    explicit Edge_attribute() {}
    explicit Edge_attribute(Attribute<T> attr) : Attribute<T>(attr) {}

    // random access data stored for edge e
    typename Attribute<T>::reference operator[](Edge e)
    {
      return Attribute<T>::operator[](e.idx());
    }

    typename Attribute<T>::const_reference operator[](Edge e) const
    {
      return Attribute<T>::operator[](e.idx());
    }
  };

  template <class T>
  class Face_attribute : public Attribute<T>
  {
  public:

    explicit Face_attribute() {}
    explicit Face_attribute(Attribute<T> attr) : Attribute<T>(attr) {}

    // random access data stored for face f
    typename Attribute<T>::reference operator[](Face f)
    {
      return Attribute<T>::operator[](f.idx());
    }

    typename Attribute<T>::const_reference operator[](Face f) const
    {
      return Attribute<T>::operator[](f.idx());
    }
  };

public://--- iterator types
  
  class Vertex_iterator
  {
  public:

    Vertex_iterator(Vertex v = Vertex(), const PolygonMesh* mesh = NULL) : hnd_(v), mesh_(mesh)
    {
      if (mesh_ && mesh_->garbage())
      {
        // if mesh has deleted vertices, edges or faces
        while (mesh->is_valid(hnd_) && mesh_->is_deleted(hnd_))
        {
          // move the iterator to the next nearest valid vertex?
          ++hnd_.idx_;
        }
      }
    }

    // get the vertex the iterator refers to
    Vertex operator*() const { return hnd_; };

    // are two iterators equal?
    bool operator==(const Vertex_iterator& rhs) const
    {
      return (hnd_ == rhs.hnd_);
    }

    // are two iterators different?
    bool operator!=(const Vertex_iterator& rhs) const
    {
      return !operator==(rhs);
    }

    // pre-increment iterator
    Vertex_iterator& operator++()
    {
      ++hnd_.idx_;
      assert(mesh_);
      while (mesh_->garbage() && mesh_->is_valid(hnd_) && mesh_->is_deleted(hnd_))
      {
        ++hnd_.idx_;
      }
      return *this;
    }

    Vertex_iterator& operator--()
    {
      --hnd_.idx_;
      assert(mesh_);
      while (mesh_->garbage() && mesh_->is_valid(hnd_) && mesh_->is_deleted(hnd_))
      {
        --hnd_.idx_;
      }
      return *this;
    }

  private:
    Vertex hnd_;
    const PolygonMesh* mesh_;
  };

  class Halfedge_iterator
  {
  public:

    // default constructor
    Halfedge_iterator(Halfedge h = Halfedge(), const PolygonMesh* mesh = NULL) : hnd_(h), mesh_(mesh)
    {
      if (mesh_ && mesh_->garbage())
      {
        while (mesh_->is_valid(hnd_) && mesh_->is_deleted(hnd_))
        {
           ++hnd_.idx_;
        }
      }
    }

    Halfedge operator*() const { return hnd_; };

    bool operator==(const Halfedge_iterator& rhs) const
    {
      return (hnd_ == rhs.hnd_);
    }

    bool operator!=(const Halfedge_iterator& rhs) const
    {
      return !operator==(rhs);
    }

    Halfedge_iterator& operator++()
    {
      ++hnd_.idx_;
      assert(mesh_);
      while (mesh_->garbage() && mesh_->is_valid(hnd_) && mesh_->is_deleted(hnd_))
      {
        ++hnd_.idx_;
      }
      return *this;
    }

    Halfedge_iterator& operator--()
    {
      --hnd_.idx_;
      assert(mesh_);
      while (mesh_->garbage() && mesh_->is_valid(hnd_) && mesh_->is_deleted(hnd_))
      {
        --hnd_.idx_;
      }
      return *this;
    }

  private:
    Halfedge hnd_;
    const PolygonMesh* mesh_;

  };

  class Edge_iterator
  {
  public:
    
    Edge_iterator(Edge e = Edge(), const PolygonMesh* mesh = NULL) : hnd_(e), mesh_(mesh)
    {
      if (mesh_ && mesh_->garbage())
      {
        while (mesh_->is_valid(hnd_) && mesh_->is_deleted(hnd_))
        {
          ++hnd_.idx_;
        }
      }
    }

    Edge operator*() const { return hnd_; };

    bool operator==(const Edge_iterator& rhs) const
    {
      return (hnd_ == rhs.hnd_);
    }

    bool operator!=(const Edge_iterator& rhs) const
    {
      return !operator==(rhs);
    }

    Edge_iterator& operator++()
    {
      ++hnd_.idx_;
      assert(mesh_);
      while (mesh_->garbage() && mesh_->is_valid(hnd_) && mesh_->is_deleted(hnd_))
      {
        ++hnd_.idx_;
      }
      return *this;
    }

    Edge_iterator& operator--()
    {
      --hnd_.idx_;
      assert(mesh_);
      while (mesh_->garbage() && mesh_->is_valid(hnd_) && mesh_->is_deleted(hnd_))
      {
        --hnd_.idx_;
      }
      return *this;
    }

  private:
    Edge hnd_;
    const PolygonMesh* mesh_;
  };

  class Face_iterator
  {
  public:

    Face_iterator(Face f = Face(), const PolygonMesh* mesh = NULL) : hnd_(f), mesh_(mesh)
    {
      if (mesh_ && mesh->garbage())
      {
        while (mesh_->is_valid(hnd_) && mesh_->is_deleted(hnd_))
        {
          ++hnd_.idx_;
        }
      }
    }

    Face operator*() const { return hnd_; };

    bool operator==(const Face_iterator& rhs) const
    {
      return (hnd_ == rhs.hnd_);
    }

    bool operator!=(const Face_iterator& rhs) const
    {
      return !operator==(rhs);
    }

    Face_iterator& operator++()
    {
      ++hnd_.idx_;
      assert(mesh_);
      while (mesh_->garbage() && mesh_->is_valid(hnd_) && mesh_->is_deleted(hnd_))
      {
        ++hnd_.idx_;
      }
      return *this;
    }

    Face_iterator& operator--()
    {
      --hnd_.idx_;
      assert(mesh_);
      while (mesh_->garbage() && mesh_->is_valid(hnd_) && mesh_->is_deleted(hnd_))
      {
        --hnd_.idx_;
      }
      return *this;
    }

  private:
    Face hnd_;
    const PolygonMesh* mesh_;
  };

public: //--- container for range-based for loops

  class Vertex_container
  {
  public:
    Vertex_container(Vertex_iterator _begin, Vertex_iterator _end) : begin_(_begin), end_(_end) {};
    Vertex_iterator begin() const { return begin_; };
    Vertex_iterator end()   const { return end_; };

  private:
    Vertex_iterator begin_, end_;
  };

  class Halfedge_container
  {
  public:
    Halfedge_container(Halfedge_iterator _begin, Halfedge_iterator _end) : begin_(_begin), end_(_end) {};
    Halfedge_iterator begin() const { return begin_; };
    Halfedge_iterator end()   const { return end_; };

  private:
    Halfedge_iterator begin_, end_;
  };

  class Edge_container
  {
  public:
    Edge_container(Edge_iterator _begin, Edge_iterator _end) : begin_(_begin), end_(_end) {};
    Edge_iterator begin() const { return begin_; };
    Edge_iterator end()   const { return end_; };

  private:
    Edge_iterator begin_, end_;
  };

  class Face_container
  {
  public:
    Face_container(Face_iterator _begin, Face_iterator _end) : begin_(_begin), end_(_end) {};
    Face_iterator begin() const { return begin_; };
    Face_iterator end()   const { return end_; };

  private:
    Face_iterator begin_, end_;
  };


public: //--- circulator types

  class Vertex_around_vertex_circulator
  {
  public:
    
    Vertex_around_vertex_circulator(const PolygonMesh* mesh = NULL, Vertex v = Vertex())
    : mesh_(mesh), active_(true)
    {
      if (mesh_)
      {
        halfedge_ = mesh_->halfedge(v);
      }
    }

    bool operator==(const Vertex_around_vertex_circulator& rhs) const
    {
      assert(mesh_);
      return (active_ && (mesh_ == rhs.mesh_) && (halfedge_ == rhs.halfedge_));
    }

    bool operator!=(const Vertex_around_vertex_circulator& rhs) const
    {
      return !operator==(rhs);
    }

    Vertex_around_vertex_circulator& operator++()
    {
      assert(mesh_);
      halfedge_ = mesh_->ccw_rotated_halfedge(halfedge_);
      active_ = true;
      return *this;
    }

    //
    Vertex_around_vertex_circulator& operator--()
    {
      assert(mesh_);
      halfedge_ = mesh_->cw_rotated_halfedge(halfedge_);
      return *this;
    }

    // get the vertex the circulator refers to
    Vertex operator*() const
    {
      assert(mesh_);
      return mesh_->to_vertex(halfedge_);
    }

    // cast to bool: true if vertex is not isolated
    operator bool() const { return halfedge_.is_valid(); };

    Halfedge halfedge() const { return halfedge_; };

    // for c++11 range-based for
    Vertex_around_vertex_circulator& begin() { active_ = !halfedge_.is_valid(); return *this; };
    // for c++11 range-based for
    Vertex_around_vertex_circulator& end() { active_ = true; return *this; };

  private:
    Halfedge halfedge_;
    const PolygonMesh* mesh_;
    // helper for c++11 range-based for-loops
    bool active_;
  };

  class Halfedge_around_vertex_circulator
  {
  public:

    Halfedge_around_vertex_circulator(const PolygonMesh* mesh = NULL, Vertex v = Vertex())
    : mesh_(mesh), active_(true)
    {
      if (mesh_) 
      {
        halfedge_ = mesh_->halfedge(v);
      }
    }

    bool operator==(const Halfedge_around_vertex_circulator& rhs) const
    {
      assert(mesh_);
      return (active_ && (mesh_ == rhs.mesh_) && (halfedge_ == rhs.halfedge_));
    }

    bool operator!=(const Halfedge_around_vertex_circulator& rhs) const
    {
      return !operator==(rhs);
    }

    Halfedge_around_vertex_circulator& operator++()
    {
      assert(mesh_);
      halfedge_ = mesh_->ccw_rotated_halfedge(halfedge_);
      active_ = true;
      return *this;
    }

    Halfedge_around_vertex_circulator& operator--()
    {
      assert(mesh_);
      halfedge_ = mesh_->cw_rotated_halfedge(halfedge_);
      active_ = true;
      return *this;
    }

    Halfedge operator*() const { return halfedge_; };

    operator bool() const { return halfedge_.is_valid(); };

    // helper for c++11 range-based for
    Halfedge_around_vertex_circulator& begin() { active_ = !halfedge_.is_valid(); return *this; };
    Halfedge_around_vertex_circulator& end() { active_ = true; return *this; };


  private:
    const PolygonMesh* mesh_;
    Halfedge       halfedge_;
    bool active_;
  };

  class Face_around_vertex_circulator
  {
  public:

    Face_around_vertex_circulator(const PolygonMesh* mesh = NULL, Vertex v = Vertex())
    : mesh_(mesh), active_(true)
    {
      if (mesh_)
      {
        halfedge_ = mesh_->halfedge(v);
        if (halfedge_.is_valid() && mesh_->is_boundary(halfedge_))
        {
          operator++();
        }
      }
    }

    bool operator==(const Face_around_vertex_circulator& rhs) const
    {
      assert(mesh_);
      return (active_ && (mesh_ == rhs.mesh_) && (halfedge_ == rhs.halfedge_));
    }

    bool operator!=(const Face_around_vertex_circulator& rhs) const
    {
      return !operator==(rhs);
    }

    Face_around_vertex_circulator& operator++()
    {
      assert(mesh_ && halfedge_.is_valid());
      do 
      {
        halfedge_ = mesh_->ccw_rotated_halfedge(halfedge_);
      } while (mesh_->is_boundary(halfedge_));
      active_ = true;
      return *this;
    }


    Face_around_vertex_circulator& operator--()
    {
      assert(mesh_ && halfedge_.is_valid());
      do 
      {
        halfedge_ = mesh_->cw_rotated_halfedge(halfedge_);
      } while (mesh_->is_boundary(halfedge_));
    }

    Face operator*() const
    {
      assert(mesh_ && halfedge_.is_valid());
      return mesh_->face(halfedge_);
    }

    operator bool() const { return halfedge_.is_valid(); };

    // TODO need to be tested
    Face_around_vertex_circulator& begin() { active_ = !halfedge_.is_valid(); return *this; };
    Face_around_vertex_circulator& end() { active_ = true; return *this; };

  private:
    const PolygonMesh* mesh_;
    Halfedge       halfedge_;
    bool active_;
  };

  class Vertex_around_face_circulator
  {
  public:

    Vertex_around_face_circulator(const PolygonMesh* mesh = NULL, Face f = Face())
    : mesh_(mesh), active_(true)
    {
      if (mesh_)
      {
        halfedge_ = mesh_->halfedge(f);
      }
    }

    bool operator==(const Vertex_around_face_circulator& rhs) const
    {
      assert(mesh_);
      return (active_ && (mesh_ == rhs.mesh_) && (halfedge_ == rhs.halfedge_));
    }

    bool operator!=(const Vertex_around_face_circulator& rhs) const
    {
      return !operator==(rhs);
    }

    Vertex_around_face_circulator& operator++()
    {
      assert(mesh_ && halfedge_.is_valid());
      halfedge_ = mesh_->next_halfedge(halfedge_);
      active_ = true;
      return *this;
    }

    Vertex_around_face_circulator& operator--()
    {
      assert(mesh_ && halfedge_.is_valid());
      halfedge_ = mesh_->prev_halfedge(halfedge_);
      return *this;
    }

    Vertex operator*() const
    {
      assert(mesh_ && halfedge_.is_valid());
      return mesh_->to_vertex(halfedge_);
    }

    Vertex_around_face_circulator& begin() { active_ = false; return *this; };
    Vertex_around_face_circulator& end() { active_ = true; return *this; };

  private:
    const PolygonMesh* mesh_;
    Halfedge       halfedge_;
    bool active_;
  };

  class Halfedge_around_face_circulator
  {
  public:

    Halfedge_around_face_circulator(const PolygonMesh* mesh = NULL, Face f = Face())
    : mesh_(mesh), active_(true)
    {
      if (mesh_)
      {
        halfedge_ = mesh_->halfedge(f);
      }
    }

    bool operator==(const Halfedge_around_face_circulator& rhs) const
    {
      assert(mesh_);
      return (active_ && (mesh_ == rhs.mesh_) && (halfedge_ == rhs.halfedge_));
    }

    bool operator!=(const Halfedge_around_face_circulator& rhs) const
    {
      return !operator==(rhs);
    }

    Halfedge_around_face_circulator& operator++()
    {
      assert(mesh_ && halfedge_.is_valid());
      halfedge_ = mesh_->next_halfedge(halfedge_);
      active_ = true;
      return *this;
    }

    Halfedge_around_face_circulator& operator--()
    {
      assert(mesh_ && halfedge_.is_valid());
      halfedge_ = mesh_->prev_halfedge(halfedge_);
      return *this;
    }

    Halfedge operator*() const { return halfedge_; };

    Halfedge_around_face_circulator& begin() { active_ = false; return *this; };
    Halfedge_around_face_circulator& end() { active_ = true; return *this; };


  private:
    const PolygonMesh* mesh_;
    Halfedge       halfedge_;
    bool active_;
  };

public: //--- constructor / destructor
  
  PolygonMesh();

  virtual ~PolygonMesh();

  PolygonMesh(const PolygonMesh& rhs) { operator=(rhs); };

  PolygonMesh& operator=(const PolygonMesh& rhs);

  //PolygonMesh& assign(const PolygonMesh& rhs);



public: //--- IO

  bool read(const std::string& filename);

  bool write(const std::string& filename) const;


public: //--- add new vertex / face

  Vertex add_vertex(const Vec3& p);

  Face add_face(const std::vector<Vertex>& vertices);

  // use add_face() API above
  Face add_triangle(Vertex v0, Vertex v1, Vertex v2);

  // use add_face() API above
  Face add_quad(Vertex v0, Vertex v1, Vertex v2, Vertex v3);


public: //--- memory management

  size_t vertices_size() const { return vattrs_.size(); };
  size_t halfedges_size() const { return hattrs_.size(); };
  size_t edges_size() const { return eattrs_.size(); };
  size_t faces_size() const { return fattrs_.size(); };

  size_t n_vertices() const { return vertices_size() - deleted_vertices_; };
  size_t n_halfedges() const { return halfedges_size() - 2 * deleted_edges_; };
  size_t n_edges() const { return edges_size() - deleted_edges_; };
  size_t n_faces() const { return faces_size() - deleted_faces_; };

  bool empty() const { return (n_vertices() == 0); };

  void clear();

  // free redundant memory, different from clear()
  void free_memory();

  void reserve(size_t nvertices,
               size_t nedges,
               size_t nfaces);

  void garbage_collection();

  // returns whether vertex v is deleted
  bool is_deleted(Vertex v) const
  {
    return vdeleted_[v];
  }

  bool is_deleted(Halfedge h) const
  {
    return edeleted_[edge(h)];
  }

  bool is_deleted(Edge e) const
  {
    return edeleted_[e];
  }

  bool is_deleted(Face f) const
  {
    return fdeleted_[f];
  }

  bool is_valid(Vertex v) const
  {
    return (0 <= v.idx()) && (v.idx() < (int)vertices_size());
  }

  bool is_valid(Halfedge h) const
  {
    return (0 <= h.idx() && (h.idx() < (int)halfedges_size()));
  }

  bool is_valid(Edge e) const
  {
    return (0 <= e.idx() && (e.idx() < (int)edges_size()));
  }

  bool is_valid(Face f) const
  {
    return (0 <= f.idx() && (f.idx() < (int)faces_size()));
  }

public: //--- low-level connectivity

  // return an outgoing hafledge of the vertex
  // standard definition for hafledge data structure
  Halfedge halfedge(Vertex v) const
  {
    return vconn_[v].halfedge_;
  }

  // set the outgoing halfedge of vertex v to h
  void set_halfedge(Vertex v, Halfedge h)
  {
    vconn_[v].halfedge_ = h;
  }

  // is a boundary vertex
  bool is_boundary(Vertex v) const
  {
    Halfedge h(halfedge(v));
    return (!(h.is_valid() && face(h).is_valid()));
  }

  // not in any face
  bool is_isolated(Vertex v) const
  {
    return !halfedge(v).is_valid();
  }

  bool is_manifold(Vertex v) const
  {
    // ? more than one outgoing boundary halfedge
    int n(0);
    Halfedge_around_vertex_circulator hit = halfedges(v), hend = hit;
    if (hit)
    {
      do 
      {
        if (is_boundary(*hit))
        {
          ++n;
        }
      } while (++hit != hend);
    }
    return n < 2;
  }

  Vertex to_vertex(Halfedge h) const
  {
    return hconn_[h].vertex_;
  }

  Vertex from_vertex(Halfedge h) const
  {
    return to_vertex(opposite_halfedge(h));
  }

  void set_vertex(Halfedge h, Vertex v)
  {
    hconn_[h].vertex_ = v;
  }

  Face face(Halfedge h) const
  {
    return hconn_[h].face_;
  }

  void set_face(Halfedge h, Face f)
  {
    hconn_[h].face_ = f;
  }

  Halfedge next_halfedge(Halfedge h) const
  {
    return hconn_[h].next_halfedge_;
  }

  void set_next_halfedge(Halfedge h, Halfedge nh)
  {
    hconn_[h].next_halfedge_ = nh;
    hconn_[nh].prev_halfedge_ = h;
  }

  Halfedge prev_halfedge(Halfedge h) const
  {
    return hconn_[h].prev_halfedge_;
  }

  Halfedge opposite_halfedge(Halfedge h) const
  {
    return Halfedge((h.idx() & 1) ? h.idx() - 1 : h.idx() + 1);
  }

  // counter clock-wise rotated halfedge
  Halfedge ccw_rotated_halfedge(Halfedge h) const
  {
    return opposite_halfedge(prev_halfedge(h));
  }

  Halfedge cw_rotated_halfedge(Halfedge h) const
  {
    return next_halfedge(opposite_halfedge(h));
  }

  Edge edge(Halfedge h) const
  {
    return Edge(h.idx() >> 1); // fast divide by 2
  }

  bool is_boundary(Halfedge h) const
  {
    return !face(h).is_valid();
  }

  // return s the ith halfedge of edge e, i is only allowed to be 0 or 1
  Halfedge halfedge(Edge e, unsigned int i) const
  {
    assert(i <= 1);
    return Halfedge((e.idx() << 1) + i);
  }

  Vertex vertex(Edge e, unsigned int i) const
  {
    assert(i <= 1);
    return to_vertex(halfedge(e, i));
  }

  Face face(Edge e, unsigned int i) const
  {
    assert(i <= 1);
    return face(halfedge(e, i));
  }

  // return if  Edge e is a boundary edge
  bool is_boundary(Edge e) const
  {
    return (is_boundary(halfedge(e, 0)) || is_boundary(halfedge(e, 1)));
  }

  Halfedge halfedge(Face f) const
  {
    return fconn_[f].halfedge_;
  }

  void set_halfedge(Face f, Halfedge h)
  {
    fconn_[f].halfedge_ = h;
  }

  bool is_boundary(Face f) const
  {
    Halfedge h = halfedge(f);
    Halfedge hh = h;
    do 
    {
      if (is_boundary(opposite_halfedge(h)))
      {
        return true;
      }
      h = next_halfedge(h);
    } while (h != hh);
    return false;
  }


public: //--- attribute handling


  template <class T>
  Vertex_attribute<T> add_vertex_attribute(const std::string& name, const T t= T())
  {
    return Vertex_attribute<T>(vattrs_.add<T>(name, t));
  }

    template <class T>
    Halfedge_attribute<T> add_halfedge_attribute(const std::string& name, const T t=T())
    {
      return Halfedge_attribute<T>(hattrs_.add<T>(name, t));
    }

    template <class T>
    Edge_attribute<T> add_edge_attribute(const std::string& name, const T t=T())
    {
      return Edge_attribute<T>(eattrs_.add<T>(name, t));
    }

    template <class T>
    Face_attribute<T> add_face_attribute(const std::string& name, const T t=T())
    {
      return Face_attribute<T>(fattrs_.add<T>(name, t));
    }

    template <class T>
    Vertex_attribute<T> get_vertex_attribute(const std::string& name) const
    {
      return Vertex_attribute<T>(vattrs_.get<T>(name));
    }

    template <class T>
    Halfedge_attribute<T> get_halfedge_attribute(const std::string& name) const
    {
      return Halfedge_attribute<T>(hattrs_.get<T>(name));
    }

    template <class T>
    Edge_attribute<T> get_edge_attribute(const std::string& name) const
    {
      return Edge_attribute<T>(eattrs_.get<T>(name));
    }

    template <class T>
    Face_attribute<T> get_face_attribute(const std::string& name) const
    {
      return Face_attribute<T>(fattrs_.get<T>(name));
    }

    template <class T>
    Vertex_attribute<T> vertex_attribute(const std::string& name)
    {
      return Vertex_attribute<T>(vattrs_.get_or_add<T>(name));
    }

    template <class T>
    Halfedge_attribute<T> halfedge_attribute(const std::string& name)
    {
      return Halfedge_attribute<T>(hattrs_.get_or_add<T>(name));
    }

    template <class T>
    Edge_attribute<T> edge_attribute(const std::string& name)
    {
      return Edge_attribute<T>(eattrs_.get_or_add<T>(name));
    }

    template <class T>
    Face_attribute<T> face_attribute(const std::string& name)
    {
      return Face_attribute<T>(fattrs_.get_or_add<T>(name));
    }

    template <class T>
    void remove_vertex_attribute(Vertex_attribute<T>& attr)
    {
      vattrs_.remove(attr);
    }

    template <class T>
    void remove_halfedge_attribute(Halfedge_attribute<T>& attr)
    {
      hattrs_.remove(attr);
    }

    template <class T>
    void remove_edge_attribute(Edge_attribute<T>& attr)
    {
      eattrs_.remove(attr);
    }

    template <class T>
    void remove_face_attribute(Face_attribute<T>& attr)
    {
      fattrs_.remove(attr);
    }

    const std::type_info& get_vertex_attribute_type(const std::string& name)
    {
      return vattrs_.get_type(name);
    }

    const std::type_info& get_halfedge_attribute_type(const std::string& name)
    {
      return hattrs_.get_type(name);
    }

    const std::type_info& get_edge_attribute_type(const std::string& name)
    {
      return eattrs_.get_type(name);
    }

    const std::type_info& get_face_attribute_type(const std::string& name)
    {
      return fattrs_.get_type(name);
    }

    // return the names of all attributes
    std::vector<std::string> vertex_attributes() const
    {
      return vattrs_.attributes();
    }

    std::vector<std::string> halfedge_attributes() const
    {
      return hattrs_.attributes();
    }

    std::vector<std::string> edge_attributes() const
    {
      return eattrs_.attributes();
    }

    std::vector<std::string> face_attributes() const
    {
      return fattrs_.attributes();
    }

    void attribute_stats() const
    {
      std::vector<std::string> props;

      std::cout << "vertex properties:\n";
      props = vertex_attributes();
      for (unsigned int i=0; i<props.size(); ++i)
        std::cout << "\t" << props[i] << std::endl;

      std::cout << "halfedge properties:\n";
      props = halfedge_attributes();
      for (unsigned int i=0; i<props.size(); ++i)
        std::cout << "\t" << props[i] << std::endl;

      std::cout << "edge properties:\n";
      props = edge_attributes();
      for (unsigned int i=0; i<props.size(); ++i)
        std::cout << "\t" << props[i] << std::endl;

      std::cout << "face properties:\n";
      props = face_attributes();
      for (unsigned int i=0; i<props.size(); ++i)
        std::cout << "\t" << props[i] << std::endl;
    }

public: //--- iterators & circulators

  Vertex_iterator vertices_begin() const
  {
    return Vertex_iterator(Vertex(0), this);
  }

  Vertex_iterator vertices_end() const
  {
    return Vertex_iterator(Vertex(vertices_size()), this);
  }

  Vertex_container vertices() const
  {
    return Vertex_container(vertices_begin(), vertices_end());
  }

  Halfedge_iterator halfedges_begin() const
  {
    return Halfedge_iterator(Halfedge(0), this);
  }

  Halfedge_iterator halfedges_end() const
  {
    return Halfedge_iterator(Halfedge(halfedges_size()), this);
  }

  Halfedge_container halfedges() const
  {
    return Halfedge_container(halfedges_begin(), halfedges_end());
  }

  Edge_iterator edges_begin() const
  {
    return Edge_iterator(Edge(0), this);
  }

  Edge_iterator edges_end() const
  {
    return Edge_iterator(Edge(edges_size()), this);
  }

  Edge_container edges() const
  {
    return Edge_container(edges_begin(), edges_end());
  }

  Face_iterator faces_begin() const
  {
    return Face_iterator(Face(0), this);
  }

  Face_iterator faces_end() const
  {
    return Face_iterator(Face(faces_size()), this);
  }

  Face_container faces() const
  {
    return Face_container(faces_begin(), faces_end());
  }

  Vertex_around_vertex_circulator vertices(Vertex v) const
  {
    return Vertex_around_vertex_circulator(this, v);
  }

  Halfedge_around_vertex_circulator halfedges(Vertex v) const
  {
    return Halfedge_around_vertex_circulator(this, v);
  }

  Face_around_vertex_circulator faces(Vertex v) const
  {
    return Face_around_vertex_circulator(this, v);
  }

  Vertex_around_face_circulator vertices(Face f) const
  {
    return Vertex_around_face_circulator(this, f);
  }

  Halfedge_around_face_circulator halfedges(Face f) const
  {
    return Halfedge_around_face_circulator(this, f);
  }


public: // ---higher-level operations

  // TODO

  Halfedge find_halfedge(Vertex start, Vertex end) const;

public: //--- geometry-related functions


  const Vec3& position(Vertex v) const { return vpoint_[v]; };

  Vec3& position(Vertex v) { return vpoint_[v]; };

  std::vector<Vec3>& points() { return vpoint_.vector(); };

  void update_face_normals();

  Vec3 compute_face_normal(Face f) const;

  void update_vertex_normals();

  Vec3 compute_vertex_normal(Vertex v) const;

  Scalar edge_length(Edge e) const;

  void update_laplacian_cot();

  Scalar compute_laplacian_cot(Edge e) const;



public: //--- allocate new elements

  Vertex new_vertex()
  {
    vattrs_.push_back();
    return Vertex(vertices_size() - 1);
  }

  Halfedge new_edge(Vertex start, Vertex end)
  {
    assert(start != end);

    eattrs_.push_back();
    hattrs_.push_back();
    hattrs_.push_back();

    Halfedge h0(halfedges_size() - 2);
    Halfedge h1(halfedges_size() - 1);

    set_vertex(h0, end);
    set_vertex(h1, start);

    return h0;
  }

  Face new_face()
  {
    fattrs_.push_back();
    return Face(faces_size() - 1);
  }

private: //--- helper functions


  // make sure the outgoing halfedge of vertex v is a boundary halfedge
  // when v is a boundary vertex
  void adjust_outgoing_halfedge(Vertex v);

  bool garbage() const { return garbage_; };

private: //------------------------------------------------------- private data

  friend bool read_poly(PolygonMesh& mesh, const std::string& filename);

  AttributeContainer vattrs_;
  AttributeContainer hattrs_;
  AttributeContainer eattrs_;
  AttributeContainer fattrs_;

  Vertex_attribute<Vertex_connectivity>      vconn_;
  Halfedge_attribute<Halfedge_connectivity>  hconn_;
  Face_attribute<Face_connectivity>          fconn_;

  Vertex_attribute<bool>  vdeleted_;
  Edge_attribute<bool>    edeleted_;
  Face_attribute<bool>    fdeleted_;

  Vertex_attribute<Vec3>   vpoint_;
  Vertex_attribute<Vec3>  vnormal_;
  Face_attribute<Vec3>    fnormal_;

  unsigned int deleted_vertices_;
  unsigned int deleted_edges_;
  unsigned int deleted_faces_;
  bool garbage_;

  // helper data for add_face()
  typedef std::pair<Halfedge, Halfedge>  NextCacheEntry;
  typedef std::vector<NextCacheEntry>    NextCache;
  std::vector<Vertex>      add_face_vertices_;
  std::vector<Halfedge>    add_face_halfedges_;
  std::vector<bool>        add_face_is_new_;
  std::vector<bool>        add_face_needs_adjust_;
  NextCache                add_face_next_cache_;
};


} // namespace LgMesh

#endif // !PolygonMesh_H
