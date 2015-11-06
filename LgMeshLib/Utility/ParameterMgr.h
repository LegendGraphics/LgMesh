#ifndef LG_PARAMETERMGR_H
#define LG_PARAMETERMGR_H

#include <memory>
#include "Parameter.h"

namespace LG {

class GlobalParameterMgr
{
private:
  GlobalParameterMgr()
  : para_container_(new GlobalParameterContainer()) {};
  GlobalParameterMgr(const GlobalParameterMgr&) {};
  GlobalParameterMgr& operator=(const GlobalParameterMgr&) {};

public:
  virtual ~GlobalParameterMgr() {};

  static GlobalParameterMgr* GetInstance()
  {
    static GlobalParameterMgr instance;
    return &instance;
  }

  template <class T>
  T& add_parameter(const std::string& name)
  {
    return para_container_->add_parameter<T>(name);
  }

  template <class T>
  T& add_parameter(const std::string& name, const T& initval)
  {
    return para_container_->add_parameter<T>(name, initval);
  }

  template <class T>
  T& get_parameter(const std::string& name)
  {
    return para_container_->get_parameter<T>(name);
  }

private:
  std::unique_ptr<GlobalParameterContainer> para_container_;
};

}

#endif