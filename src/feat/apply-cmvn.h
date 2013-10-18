#ifndef BARISTA_APPLY_CMVN_H_
#define BARISTA_APPLY_CMVN_H_

#include "base/module-base.h"

#include <transform/cmvn.h>

class ApplyCMVN : public ModuleBase {
public:
  ApplyCMVN(const std::string& name_,
            const cppa::group_ptr& subscribers_,
            const boost::property_tree::ptree& config) :
    ModuleBase(name_, subscribers_) {
    
    // config
    norm_vars = config.get<bool>(name + ".norm_vars");
    context_size = config.get<kaldi::uint32>(name + ".context_size");
    
    buffer_size = 2 * context_size + 1;
    head = 0;
    current = 0;
    out_size = 0;

    running_state = Run();
  }
    
  cppa::behavior Run();

private:
  bool NormalizeFrames();
    
  // configurable members
  bool norm_vars;
  kaldi::uint32 context_size;
  
  // class internal members
  kaldi::Matrix<float> buffer;    // circular buffer
  kaldi::uint32 buffer_size;      
  kaldi::uint32 head;             // keeps track of head frame
  kaldi::uint32 current;          // keeps track of first frame to normalize
  kaldi::uint32 last;             // keeps track of last frame to normalize
  bool full;                      // buffer full or not
  
  kaldi::Matrix<float> out_data;  // normalized frames
  kaldi::uint32 out_size;         // number of normalized frames
  
  kaldi::Matrix<double> cmvn_stats;  
};

#endif
