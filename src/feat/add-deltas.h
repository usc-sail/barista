#ifndef BARISTA_ADD_DELTAS_H_
#define BARISTA_ADD_DELTAS_H_

#include "base/module-base.h"

#include <feat/feature-functions.h>

class AddDeltas : public ModuleBase {
public:
  AddDeltas(const std::string& name_,
            const cppa::group_ptr& subscribers_,
            const boost::property_tree::ptree& config) :
    ModuleBase(name_, subscribers_) {
    
    // config
    // order = config.get<bool>(name + ".order");
    // window = config.get<bool>(name + ".window");
    
    // FIXME: Here we should set the delta opts according to config file
    
    // opts.order: Order of delta computation
    // opts.window: Parameter controlling window for delta computation 
    // (actual window size for each delta order is 2 * window +  1)
    // The behavior at the edges is to replicate the first or last frame.
    
    // pad_size: how many contextual frames we need for correct computation
    pad_size = 2 * opts.order * opts.window;
    
    pad_feats(0, 0);
    
    running_state = Run();
  }
    
  cppa::behavior Run();

private:  
  // configurable members
  bool norm_vars;
  
  // class internal members
  kaldi::DeltaFeaturesOptions opts;
  kaldi::MatrixIndexT pad_size;
  
  kaldi::Matrix<float> pad_feats;
  kaldi::Matrix<float> delta_feats;
  
  kaldi::Matrix<float> out_data;
  
};

#endif
