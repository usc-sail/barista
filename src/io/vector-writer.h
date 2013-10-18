#ifndef BARISTA_VECTOR_WRITER_H_
#define BARISTA_VECTOR_WRITER_H_

#include "base/module-base.h"

class VectorWriter : public ModuleBase {
public:
  VectorWriter(const std::string& name_,
               const cppa::group_ptr& subscribers_,
               const boost::property_tree::ptree& config) :
    ModuleBase(name_, subscribers_) {
    // config
    sink_file = config.get<std::string>(name + ".sink_file");
    
    // initialize
    sink = new std::ofstream(sink_file.c_str());
    cppa::aout << " ++  " << name << ": sink file: " << sink_file << std::endl;
    
    running_state = Run();
  }
  
  ~VectorWriter() {
    sink->close();
    delete sink;
  }
  
  cppa::behavior Run();

private:
  // configurable members
  std::string sink_file;
  
  // class internal members
  std::ofstream* sink;
};

#endif
