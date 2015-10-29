#ifndef PolygonMesh_H
#define PolygonMesh_H

#include "Attributes.h"
#include "Kernel.h"
#include "LgMeshTypes.h"

namespace LgMesh {


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
  class Vertex_attribute
  {
    explicit Vertex_attribute() {}
    explicit Vertex_attribute(Attribute<T> attr) : Attribute<T>(attr) {}

    // random access the data stored for vertex v
    typename Attribute<T>::reference operator[](Vertex v)
    {
      return Attribute<T>::operator[](v.idx());
    }

    typename Attribute<T>::const_reference operator[](Vertex v)
    {
      return Attribute<T>::operator[](v.idx());
    }
  };

  template <class T>
  class Halfedge_attribute
  {
    explicit Halfedge_attribute() {}
    explicit Halfedge_attribute(Attribute<T> attr) : Attribute<T>(attr) {}

    // random access the data stored for halfedge he
    typename Attribute<T>::reference operator[](Halfedge he)
    {
      return Attribute<T>::operator[](he.idx());
    }

    typename Attribute<T>::const_reference operator[](Halfedge he)
    {
      return Attribute<T>operator[](he.idx());
    }
  };

  template <class T>
  class Edge_attribute
  {
    explicit Edge_attribute() {}
    explicit Edge_attribute(Attribute<T> attr) : Attribute<T>(attr) {}

    // random access data stored for edge e
    typename Attribute<T>::reference operator[](Edge e)
    {
      return Attribute<T>::operator[](e.idx());
    }

    typename Attribute<T>::const_reference operator[](Edge e)
    {
      return Attribute<T>::operator[](e.idx());
    }
  };

  template <class T>
  class Face_attrbute
  {
    explicit Face_attrbute() {}
    explicit Face_attrbute(Attribute<T> attr) : Attribute<T>(attr) {}

    // random access data stored for face f
    typename Attribute<T>::reference operator[](Face f)
    {
      return Attribute<T>::operator[](f.idx());
    }

    typename Attribute<T>::const_reference operator[](Face f)
    {
      return Attribute<T>::operator[](f.idx());
    }
  };

public://--- iterator types
  
  class Vertex_iterator
  {
  public:

    Vertex_iterator(Vertex v = Vertex(), const PolygonMesh* mesh = NULL) : hnd_(v), mesh_(m)
    {
      if (mesh_ && mesh_->garbage())
      {
        // if mesh has deleted vertices, edges or faces
        while (mesh->is_valid(hnd_) && mesh_->is_deleted(hnd_))
        {
          // move the iterator to the next nearest valid vertex?
          ++hnd_.idx;
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
    }

    Vertex_iterator& operator--()
    {
      --hnd_.idx_;
      assert(mesh_);
      while (mesh_->garbage() && mesh_->is_valid(hnd_) && mesh_->is_deleted(hnd_))
      {
        --hnd_.idx_;
      }
    }

  private:
    Vertex hnd_;
    const PolygonMesh* mesh_;
  };

  class Halfedge_iterator
  {

  };

  class Edge_iterator
  {

  };

  class Face_iterator
  {

  };


private: //--- helper functions

  bool garbage() const { return garbage_; };

private:

  bool garbage_;
};


} // namespace LgMesh

#endif // !PolygonMesh_H
