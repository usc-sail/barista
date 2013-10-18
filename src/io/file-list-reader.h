#ifndef BARISTA_FILE_LIST_READER_H_
#define BARISTA_FILE_LIST_READER_H_

#include "base/module-base.h"

class FileListReader : public ModuleBase {
public:
  FileListReader(const std::string& name_, 
                 const cppa::group_ptr& subscribers_, 
                 const boost::property_tree::ptree& config) :
    ModuleBase(name_, subscribers_) {
    
    file_list = config.get<std::string>(name + ".file_list");
    cppa::aout << " ++  " << name << ": file list: " << file_list << std::endl;
    
    running_state = Run();
  }
  
  cppa::behavior Run();

private:
  std::string file_list;
};

#endif
