#ifndef BARISTA_PORTAUDIO_READER_H_
#define BARISTA_PORTAUDIO_READER_H_

#include "base/module-base.h"

#include "base/kaldi-types.h"
#include "matrix/kaldi-vector.h"
#include "online/online-audio-source.h"

class PaReader : public ModuleBase {
public:
  PaReader(const std::string& name_, 
           const cppa::group_ptr& subscribers_, 
           const boost::property_tree::ptree& config) :
    ModuleBase(name_, subscribers_) {
    // config
    timeout = 0;
    sample_freq = 16000;  // Input sampling frequency is fixed to 16KHz  
    ring_size  = config.get<kaldi::uint32>(name + ".ring_size",  32768);
    report_int = config.get<kaldi::uint32>(name + ".report_int", 4);
    samples_per_chunk = 4560; // = frame_size_ + (nvec - 1) * frame_shift_
    
    // initialize other member variables
    audio_src = new kaldi::OnlinePaSource(timeout, sample_freq, ring_size, 
                                          report_int);
    data.Resize(samples_per_chunk);
    
    active = false;
    
    running_state = Run();
  }
  
  ~PaReader() {
    delete audio_src;
  }

  cppa::behavior Run();

private:

  // configurable members
  kaldi::uint32 timeout;      // Audio acquisition timeout
  kaldi::int32 sample_freq;   // Input sampling frequency
  kaldi::int32 ring_size;     // PortAudio's internal ring buffer size in bytes
  kaldi::int32 report_int;    // Report interval for PortAudio buffer overflows
                              // in number of feature batches
  kaldi::int32 samples_per_chunk; // number of features decoded in one go

  // class internal members
  kaldi::OnlinePaSource* audio_src;
  kaldi::Vector<float> data;      // output pcm data (represented as floats)
  
  bool active;
};

#endif
