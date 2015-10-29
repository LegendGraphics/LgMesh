#ifndef LGMESH_ATTRIBUTES_H
#define LGMESH_ATTRIBUTES_H

#include <algorithm>
#include <assert.h>
#include <iostream>
#include <string>
#include <typeinfo>
#include <vector>

namespace LgMesh {


class BaseAttributeArray
{
public:
  
  // Default constructor
  BaseAttributeArray(const std::string& name) : name_(name) {}

  // Destructor
  virtual ~BaseAttributeArray() {}

  // Reserve memory for n elements
  virtual void reserve(size_t n) = 0;

  // Resize storage to hold n elements
  virtual void resize(size_t n) = 0;

  // Free unused memory
  virtual void free_memory() = 0;

  // Add a new element
  virtual void push_back() = 0;

  // Swap storage place of two elements
  virtual void swap(size_t i0, size_t i1) = 0;

  // Return a deep copy of itself
  virtual BaseAttributeArray* clone() const = 0;

  // Return the type_info of the attribute
  virtual const std::type_info& type() = 0;

  // Return the name of the attribute
  const std::string& name() const { return name_; };

protected:
  std::string name_;
};


template <class T>
class AttributeArray : public BaseAttributeArray
{
public:

  typedef T                                      value_type;
  typedef std::vector<value_type>                vector_type;
  typedef typename vector_type::reference        reference;
  typedef typename vector_type::const_reference  const_reference;

  AttributeArray(const std::string& name, T t = T()) : BaseAttributeArray(name), value_(t) {}

public: // virtual interface of BaseAttributeArray

  virtual void reserve(size_t n)
  {
    data_.reserve(n);
  }

  virtual void resize(size_t n)
  {
    data_.resize(n, value_);
  }

  virtual void push_back()
  {
    data_.push_back(value_);
  }

  virtual void free_memory()
  {
    // free unused memory
    vector_type(data_).swap(data_);
  }

  virtual void swap(size_t i0, size_t i1)
  {
    T d(data_[i0]);
    data_[i0] = data_[i1];
    data_[i1] = d;
  }

  virtual BaseAttributeArray* clone() const
  {
    AttributeArray<T>* attr = new AttributeArray<T>(name_, value_);
    p->data_ = data_;
    return p;
  }

  virtual const std::type_info& type() { return typeid(T); };


public:

  // Get pointer to array (does not work for T == bool)
  const T* data() const
  {
    return &data_[0];
  }

  // Get reference to the underlying vector
  std::vector<T>& vector()
  {
    return data_;
  }

  // Access the i'th element. No range check is performed!
  reference operator[](int _idx)
  {
    assert( size_t(_idx) < data_.size() );
    return data_[_idx];
  }

  const_reference operator[](int _idx) const
  {
    assert( size_t(_idx) < data_.size() );
    return data_[_idx];
  }

private:
  vector_type data_;
  value_type  value_;
};

// Specialization for bool attributes
template <>
inline const bool*
AttributeArray<bool>::data() const
{
  assert(false);
  return NULL;
}

template <class T>
class Attribute
{
public:
  
  typedef typename AttributeArray<T>::reference        reference;
  typedef typename AttributeArray<T>::const_reference  const_reference;

  friend class AttributeContainer;
  friend class PolygonMesh;

public:

  Attribute(AttributeArray<T>* p = NULL) : attr_array_(p) {}

  void reset()
  {
    attr_array_ = NULL;
  }

  operator bool() const
  {
    return attr_array_ != NULL;
  }

  reference operatorp[](int i)
  {
    assert(attr_array_ != NULL);
    return (*attr_array_)[i];
  }

  const_reference operatro[](int i) const
  {
    assert(attr_array_ != NULL);
    return (*attr_array_)[i];
  }

  const T* data() const
  {
    assert(attr_array_ != NULL);
    return attr_array_->data();
  }

  std::vector<T>& vector()
  {
    assert(attr_array_ != NULL);
    return attr_array_->vector();
  }

private:
  
  AttributeArray<T>& array()
  {
    assert(attr_array_ != NULL);
    return *attr_array_;
  }

  const AttributeArray<T>& array() const
  {
    assert(attr_array_ != NULL);
    return *attr_array_;
  }

private:
  AttributeArray<T>* attr_array_;
};

class AttributeContainer
{
public:
  
  // Default constructor
  AttributeContainer() : size_(0) {}

  // Destructor (deletes all attribute arrays)
  virtual ~AttributeContainer() { clear(); };

  // Copy constructor: performs deep copy of attribute arrays
  AttributeContainer(const AttributeContainer& _rhs) { operator=(_rhs); };

  // Assignment: performs deep copy of attribute arrays
  AttributeContainer& operator=(const AttributeContainer& _rhs)
  {
    if (this != &_rhs)
    {
      clear();
      attr_arrays_.resize(_rhs.n_attributes());
      size_ = _rhs.size();
      for (size_t i = 0; i < attr_arrays_.size(); ++i)
      {
        attr_arrays_[i] = _rhs.attr_arrays_[i]->clone();
      }
    }
    return *this;
  }

  // Return the current size of the attribute arrays
  size_t size() const { return size_; };

  // Return the number of attribute arrays
  size_t n_attributes() const { return attr_arrays_.size(); };

  // Return a vector of all attribute names
  std::vector<std::string> attributes() const
  {
    std::vector<std::string> names;
    for (size_t i = 0; i < attr_arrays_.size(); ++i)
    {
      names.push_back(attr_arrays_[i]->name());
    }
    return names;
  }

  // Add an attribute with name and default value
  template <class T>
  Attribute<T> add(const std::string& name, const T t = T())
  {
    // if an attribute with this name already exists, return an invalid attribute
    for (size_t i = 0; i < attr_arrays_.size(); ++i)
    {
      if (attr_arrays_[i]->name() == name)
      {
        std::cerr << "[AttributeContainer] An attribute with name \""
                  << name << "\" already exists. Returning invalid attribute.\n";
        return Attribute<T>();
      }
    }

    // otherwise add the attribute
    AttributeArray<T>* attr = new AttributeArray<T>(name, t);
    attr->resize(size_);
    attr_arrays_.push_back(attr);
    return Attribute<T>(attr);
  }

  // Get an attribute by its name. Returns invalid attribute if it does not exist.
  template <class T>
  Attribute<T> get(const std::string& name) const
  {
    for (size_t i = 0; i < attr_arrays_.size(); ++i)
    {
      if (attr_arrays_[i]->name() == name)
      {
        return Attribute<T>(dynamic_cast<AttributeArray<T>*>(attr_arrays_[i]));
      }
    }
    return Attribute<T>();
  }

  // Returns an attribute if it exists, otherwise it creates it first.
  template <class T>
  Attribute<T> get_or_add(const std::string& name, const T t = T())
  {
    Attribute<T> attr = get<T>(name);
    if (!attr)
    {
      attr = add<T>(name, t);
    }
    return attr;
  }

  // Get the type of property by its name. Return typeid(void) if it does not exist
  const std::type_info& get_type(const std::string& name)
  {
    for (size_t i = 0; i < attr_arrays_.size(); ++i)
    {
      if (attr_arrays_[i]->name() == name)
      {
        return attr_arrays_[i]->type();
      }
    }
    return typeid(void);
  }

  // Delete an attribute
  template <class T>
  void remove(Attribute<T>& h)
  {
    std::vector<BaseAttributeArray*>::iterator it = attr_arrays_.begin();
    std::vector<BaseAttributeArray*>::iterator end = attr_arrays_.end();
    for (; it != end; ++it)
    {
      if (*it == h.attr_array_)
      {
        delete *it;
        attr_arrays_.erase(it);
        h.reset();
        break;
      }
    }
  }

  // Delete all attributes
  void clear()
  {
    for (size_t i = 0; i < attr_arrays_.size(); ++i)
    {
      delete attr_arrays_[i];
    }
    attr_arrays_.clear();
    size_ = 0;
  }

  // Reserve memory for n entries in all arrays
  void reserve(size_t n) const
  {
    for (size_t i = 0; i < attr_arrays_.size(); ++i)
    {
      attr_arrays_[i]->reserve(n);
    }
    //size_ = n;
  }

  // Resize all arrays to size n
  void resize(size_t n)
  {
    for (size_t i = 0; i < attr_arrays_.size(); ++i)
    {
      attr_arrays_[i]->resize(n);
    }
    size_ = n;
  }

  // Free unused memory space in all arrays
  void free_memory() const
  {
    for (size_t i = 0; i < attr_arrays_.size(); ++i)
    {
      attr_arrays_[i]->free_memory();
    }
  }


  // Add a new element to each vector
  void push_back()
  {
    for (size_t i = 0; i < attr_arrays_.size(); ++i)
    {
      attr_arrays_[i]->push_back();
    }
    ++size_;
  }

  // Swap elements i0 and i1 in all arrays
  void swap(size_t i0, size_t i1) const
  {
    for (size_t i = 0; i < attr_arrays_.size(); ++i)
    {
      attr_arrays_[i]->swap(i0, i1);
    }
  }


private:
  std::vector<BaseAttributeArray*> attr_arrays_;
  size_t size_; // all AttributeArray have the same size in an AttributeContainer
};

}

#endif // !LGMESH_ATTRIBUTES_H
