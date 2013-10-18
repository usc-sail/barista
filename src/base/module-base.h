#ifndef BARISTA_MODULE_BASE_H_
#define BARISTA_MODULE_BASE_H_

#include "common.h"

class ModuleBase : public cppa::sb_actor<ModuleBase> {
public:
  std::string name;
  cppa::group_ptr subscribers;
  
  cppa::behavior running_state;
  cppa::behavior config_state;
  cppa::behavior& init_state = config_state;
  
  ModuleBase(const std::string& name_, const cppa::group_ptr& subscribers_) :
    name(name_), subscribers(subscribers_) {
    config_state = Config();
  }
  
  cppa::behavior Config();

private:
  friend class cppa::sb_actor<ModuleBase>;
};

#endif
