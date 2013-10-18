#include "compute-mfcc-feats.h"

using namespace std;
using namespace kaldi;
using namespace cppa;

behavior ComputeMFCCFeats::Run(){
  return (
    on<string, Vector<float> >() >> [=](string key, Vector<float> in_data) {
      ComputeMFCC(key, in_data);
    },
    on(atom("UTT_END")) >> [=]() {
      waveform.Resize(0);
      send(subscribers, atom("UTT_END"));
    },
    on(atom("QUIT")) >> [=]() {
      self->quit(exit_reason::user_shutdown);
    },
    on(atom("DONE")) >> [=]() {
      send(subscribers, atom("DONE"));
      self->quit(exit_reason::normal);
    }
  );
}

bool ComputeMFCCFeats::ComputeMFCC(string& key, Vector<float>& in_data) {
  
  // add new wave data at the end of waveform
  waveform.Resize(waveform.Dim() + in_data.Dim(), kaldi::kCopyData);
  SubVector<float>(waveform, waveform.Dim() - in_data.Dim(), in_data.Dim()).CopyFromVec(in_data);

  // if the waveform data length is less than window length, continue to get data
  if (waveform.Dim() < frame_window_length)
    return false;

  // after computing features of the current waveform data, 
  // save the remainder for the next chunk            
  Vector<float> waveform_remainder;
  mfcc->Compute(waveform, vtln_warp_local, &out_data, &waveform_remainder);
  waveform.Swap(&waveform_remainder);
  
  send(subscribers, key, out_data);
  return true;
}

