#ifndef BARISTA_PCM_READER_H_
#define BARISTA_PCM_READER_H_

#include "base/module-base.h"

#include <base/kaldi-types.h>
#include <matrix/kaldi-vector.h>

class PCMReader : public ModuleBase {
public:
  PCMReader(const std::string& name_, 
            const cppa::group_ptr& subscribers_, 
            const boost::property_tree::ptree& config) :
    ModuleBase(name_, subscribers_) {
    // config
    bits_per_sample = config.get<kaldi::uint32>(name + ".bits_per_sample");
    samples_per_chunk = config.get<kaldi::uint32>(name + ".samples_per_chunk");
    
    // initialize other member variables
    bytes_per_sample = bits_per_sample / 8;
    bytes_per_chunk  = bytes_per_sample * samples_per_chunk;
    chunk_data = new char[bytes_per_chunk];
    data(samples_per_chunk);
    
    running_state = Run();
  }
  
  ~PCMReader() {
    delete chunk_data;
  }

  cppa::behavior Run();

private:
  bool ReadPCMFile(std::string& key, std::string& file_name);
  bool ReadPCMChunk(std::ifstream& pcm_file);

  // configurable members
  kaldi::uint32 bits_per_sample;
  kaldi::uint32 samples_per_chunk;
  
  // class internal members
  kaldi::uint32 bytes_per_sample;
  kaldi::uint32 bytes_per_chunk;
  char* chunk_data;               // binary chunk read from file
  kaldi::Vector<float> data;      // output pcm data (represented as floats)
};

#endif
