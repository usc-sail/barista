#ifndef BARISTA_COMMAND_LINE_INTERFACE_H_
#define BARISTA_COMMAND_LINE_INTERFACE_H_

#include "base/module-base.h"

class CommandLineInterface : public ModuleBase {
public:
  CommandLineInterface(const std::string& name_, 
                       const cppa::group_ptr& subscribers_, 
                       const boost::property_tree::ptree& config) :
    ModuleBase(name_, subscribers_) {    
    running_state = Run();
  }
  
  cppa::behavior Run();

private:

};

#endif
