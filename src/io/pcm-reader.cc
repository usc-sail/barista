#include "pcm-reader.h"

using namespace std;
using namespace kaldi;
using namespace cppa;

// Defines the running behavior:
behavior PCMReader::Run(){
  return (
    on<string, string>() >> [=](string key, string file_name) {
      ReadPCMFile(key, file_name);
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

// Reads a PCM file and publishes its content chunk by chunk.
bool PCMReader::ReadPCMFile(string& key, string& file_name) {
  ifstream pcm_file(file_name.c_str(), ios::binary);
  while(pcm_file.good()) {
    ReadPCMChunk(pcm_file);
    send(subscribers, key, data);
  }
  send(subscribers, atom("UTT_END"));
  pcm_file.close();
  return true;
}

// Reads a chunk of PCM data from the input stream,
// converts it to a vector of floats and stores the result in 'data'.
bool PCMReader::ReadPCMChunk(ifstream& pcm_file) {
  char *data_ptr = chunk_data;
  pcm_file.read(data_ptr, bytes_per_chunk);
  uint32 num_bytes_read = pcm_file.gcount();  // Get how many bytes were read
  if (num_bytes_read == 0)
    return false;
  // TODO: assert: num_bytes_read % bytes_per_sample == 0
  uint32 num_samples_read = num_bytes_read / bytes_per_sample;
  data.Resize(num_samples_read);
  for (uint32 i = 0; i < num_samples_read; ++i) {
    switch (bits_per_sample) {
      case 8:
        data(i) = *data_ptr;
        data_ptr++;
        break;
      case 16:
      {
        int16 k = *reinterpret_cast<uint16*>(data_ptr);
#ifdef __BIG_ENDIAN__
        KALDI_SWAP2(k);
#endif
        data(i) =  k;
        data_ptr += 2;
        break;
      }
      case 32:
      {
        int32 k = *reinterpret_cast<uint32*>(data_ptr);
#ifdef __BIG_ENDIAN__
        KALDI_SWAP4(k);
#endif
        data(i) =  k;
        data_ptr += 4;
        break;
      }
      default:
        KALDI_ERR << "ReadPCMChunk: bits_per_sample = " << bits_per_sample;
    }
  }
  return true;
}
