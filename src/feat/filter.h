#ifndef BARISTA_FILTER_H_
#define BARISTA_FILTER_H_

#include "base/module-base.h"

// Dummy filter which simply multiplies input vector by a scalar weight.
// It can be used as a template for implementing new Barista modules.

class Filter : public ModuleBase {
public:
  Filter(const std::string& name_,
         const cppa::group_ptr& subscribers_,
         const boost::property_tree::ptree& config) :
    ModuleBase(name_, subscribers_) {
    // config
    weight = config.get<float>(name + ".weight");
    
    running_state = Run();
  }
  
  ~Filter() {}
  
  cppa::behavior Run();

private:
  // configurable members
  float weight;

};

#endif
