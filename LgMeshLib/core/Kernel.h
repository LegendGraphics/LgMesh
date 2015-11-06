#ifndef Kernel_H
#define Kernel_H

#include <map>
#include <stdexcept>
#include <string>

namespace LG {

// Base class for Mesh
// Support for adding global attributes to Mesh
class Kernel
{
  // Base Global Attribute
  // support runtime type check
  class BaseGlobalAttribute
  {
  public:
    const std::type_info& mytype;
    BaseGlobalAttribute(const std::type_info& mytype = typeid(void))
      : mytype(mytype) {}
  };

  // Template class for GlobalAttribute
  template <class T>
  class GlobalAttribute : public BaseGlobalAttribute
  {
  public:
    T value;
    GlobalAttribute() : BaseGlobalAttribute(typeid(T)) {}
  };

private:
  typedef std::map<std::string, BaseGlobalAttribute*> AttributesMap;
  AttributesMap global_attrs_;

public:
  ~Kernel()
  {
    for (AttributesMap::iterator it = global_attrs_.begin(); it != global_attrs_.end(); ++it)
    {
      delete (it->second);
    }
  }

public:
  // Generic operations for adding new attribute and get existed attribute
  
  // Initialization which returns reference to value
  template <class T>
  T& add_attribute(const std::string& name)
  {
    if (global_attrs_.find(name) != global_attrs_.end())
    {
      throw std::runtime_error("Attempted to create attribute with same name");
    }
    GlobalAttribute<T>* attr = new GlobalAttribute<T>();
    global_attrs_[name] = attr;
    return attr->value;
  }

  // Immediate initialization
  template <class T>
  T& add_attribute(const std::string& name, const T& initval)
  {
    T& val = add_attribute<T>(name);
    val = initval;
    return val;
  }

  // Get an attribute by its name
  template <class T>
  T& get_attribute(const std::string& name)
  {
    if (global_attrs_.find(name) == global_attrs_.end())
    {
      throw std::runtime_error("Cannot find attribute");
    }
    if (global_attrs_[name]->mytype != typeid(T))
    {
      throw std::runtime_error("Attribute of desired type not match");
    }
    GlobalAttribute<T>* attr = static_cast<GlobalAttribute<T>*>(global_attrs_[name]);
    return attr->value;
  }
};

}

#endif // !Kernel_H
