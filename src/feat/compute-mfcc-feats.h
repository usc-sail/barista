#ifndef BARISTA_COMPUTE_MFCC_FEATS_H_
#define BARISTA_COMPUTE_MFCC_FEATS_H_

#include "base/module-base.h"

#include <feat/feature-mfcc.h>

class ComputeMFCCFeats : public ModuleBase {
public:
  ComputeMFCCFeats(const std::string& name_,
                   const cppa::group_ptr& subscribers_,
                   const boost::property_tree::ptree& config) :
    ModuleBase(name_, subscribers_) {
    // config
    use_energy = config.get<bool>(name + ".use_energy");
    vtln_warp_local = config.get<kaldi::BaseFloat>(name + ".vtln_warp_local");
    
    // Set MFCC options and allocate an MFCC extractor
    kaldi::MfccOptions op;
    op.use_energy = use_energy;
    frame_window_length = op.frame_opts.WindowSize();
    mfcc = new kaldi::Mfcc(op);

    //initialize
    waveform(0);
    out_data(0, 0);

    running_state = Run();
  }
  
  ~ComputeMFCCFeats() {
    delete mfcc;
  }
  
  cppa::behavior Run();

private:
  bool ComputeMFCC(std::string& key, kaldi::Vector<kaldi::BaseFloat>& in_data);
  
  // configurable members
  bool use_energy;
  kaldi::BaseFloat vtln_warp_local;

  // class internal members
  kaldi::Mfcc* mfcc;
  
  kaldi::uint32 frame_window_length; 
  
  kaldi::Vector<kaldi::BaseFloat> waveform;     // holds the input waveform
  kaldi::Matrix<kaldi::BaseFloat> out_data;     // holds the features
};

#endif
