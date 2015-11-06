#ifndef LG_PARAMETER_H
#define LG_PARAMETER_H

#include <map>
#include <stdexcept>
#include <string>

namespace LG {

// TODO: implement range base for loop in the future
// first need to implement ParameterIterator
class GlobalParameterContainer
{
  // Base Global Parameter
  // support runtime type check
  class BaseGlobalParameter
  {
  public:
    const std::type_info& mytype;
    BaseGlobalParameter(const std::type_info& mytype = typeid(void))
    : mytype(mytype) {}
  };

  // Template class for GlobalParameter
  template <class T>
  class GlobalParameter : public BaseGlobalParameter
  {
  public:
    T value;
    GlobalParameter() : BaseGlobalParameter(typeid(T)) {}
  };

private:
  typedef std::map<std::string, BaseGlobalParameter*> ParametersMap;
  ParametersMap global_paras_;

public:
  ~GlobalParameterContainer()
  {
    for (ParametersMap::iterator it = global_paras_.begin(); it != global_paras_.end(); ++it)
    {
      delete (it->second);
    }
  }

public:
  // Generic operators for adding new parameter and get existed parameter

  template <class T>
  T& add_parameter(const std::string& name)
  {
    if (global_paras_.find(name) != global_paras_.end())
    {
      throw std::runtime_error("Attempted to create attribute with same name");
    }
    GlobalParameter<T>* para = new GlobalParameter<T>();
    global_paras_[name] = para;
    return para->value;
  }

  template <class T>
  T& add_parameter(const std::string& name, const T& initval)
  {
    T& val = add_parameter<T>(name);
    val = initval;
    return val;
  }

  template <class T>
  T& get_parameter(const std::string& name)
  {
    if (global_paras_.find(name) == global_paras_.end())
    {
      throw std::runtime_error("Cannot find attribute");
    }
    if (global_paras_[name]->mytype != typeid(T))
    {
      throw std::runtime_error("Parameter of desired type not match");
    }
    GlobalParameter<T>* para = static_cast<GlobalParameter<T>*>(global_paras_[name]);
    return para->value;
  }
};

}

#endif // !LG_PARAMETER_H