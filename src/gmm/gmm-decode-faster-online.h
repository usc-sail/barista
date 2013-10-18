#ifndef BARISTA_GMM_DECODE_FASTER_ONLINE_H_
#define BARISTA_GMM_DECODE_FASTER_ONLINE_H_

#include "base/module-base.h"
#include "decoder/faster-online-decoder.h"

#include "base/kaldi-common.h"
#include "util/common-utils.h"
#include "gmm/am-diag-gmm.h"
#include "gmm/decodable-am-diag-gmm.h"
#include "hmm/transition-model.h"
#include "fstext/fstext-lib.h"
#include "util/timer.h"
#include "lat/kaldi-lattice.h" // for CompactLatticeArc

fst::Fst<fst::StdArc>* ReadNetwork(std::string& filename);

class GMMDecodeFasterOnline : public ModuleBase {
public:
  GMMDecodeFasterOnline(const std::string& name_,
                        const cppa::group_ptr& subscribers_,
                        const boost::property_tree::ptree& config) :
                        ModuleBase(name_, subscribers_) {

    // config
    word_syms_filename = config.get<std::string>(name + ".word_syms_filename");
    model_rxfilename = config.get<std::string>(name + ".model_rxfilename");
    fst_rxfilename = config.get<std::string>(name + ".fst_rxfilename");
    
    words_wspecifier = config.get<std::string>(name + ".words_wspecifier");
    alignment_wspecifier = config.get<std::string>(name + ".alignment_wspecifier");
    lattice_wspecifier = config.get<std::string>(name + ".lattice_wspecifier");
    
    acoustic_scale = config.get<float>(name + ".acoustic_scale");
    beam = config.get<float>(name + ".beam");
    max_active = config.get<int>(name + ".max_active");
    allow_partial = config.get<bool>(name + ".allow_partial");
    
    // set decoder options
    decoder_opts.beam = beam;
    decoder_opts.max_active = max_active; 
    
    // read acoustic model
    bool binary;
    kaldi::Input ki(model_rxfilename, &binary);
    trans_model.Read(ki.Stream(), binary);
    am_gmm.Read(ki.Stream(), binary);
    
    // setup output writers
    words_wspecifier     != "" && words_writer.Open(words_wspecifier);
    alignment_wspecifier != "" && alignment_writer.Open(alignment_wspecifier);
    lattice_wspecifier   != "" && clat_writer.Open(lattice_wspecifier);
    
    // read word symbols
    word_syms = NULL;
    if (word_syms_filename != "") 
        if (!(word_syms = fst::SymbolTable::ReadText(word_syms_filename)))
            KALDI_ERR << "Could not read symbol table from file "
            << word_syms_filename;
    

    // It's important that we initialize decode_fst after feature_reader, as it
    // can prevent crashes on systems installed without enough virtual memory.
    // It has to do with what happens on UNIX systems if you call fork() on a
    // large process: the page-table entries are duplicated, which requires a
    // lot of virtual memory.

    // read decoding network
    decode_fst = ReadNetwork(fst_rxfilename);
    
    // initialize the decoder
    decoder = new kaldi::FasterOnlineDecoder(*decode_fst, decoder_opts);

    // initialize counters, etc.
    tot_like = 0.0;
    frame_count = 0;
    num_success = 0;
    num_fail = 0;

    utt_key = "";    
    prev_utt_key = "";
    utt_frame_count = 0;
    utt_number = 1;
    
    timer.Reset();
    
    running_state = Run();
    
  }
  
  ~GMMDecodeFasterOnline() {
    delete word_syms;
    delete decoder;
    delete decode_fst;
  }
  
  cppa::behavior Run();
  
private:
  
  // configurable members
  std::string word_syms_filename;
  std::string model_rxfilename;
  std::string fst_rxfilename;

  std::string words_wspecifier;
  std::string alignment_wspecifier;
  std::string lattice_wspecifier;

  float acoustic_scale;
  float beam;
  kaldi::int32 max_active;
  bool allow_partial;  
  
  // class internal members
  kaldi::FasterOnlineDecoderOptions decoder_opts;
  kaldi::TransitionModel trans_model;
  kaldi::AmDiagGmm am_gmm;

  kaldi::Int32VectorWriter words_writer;
  kaldi::Int32VectorWriter alignment_writer;
  kaldi::CompactLatticeWriter clat_writer;
  
  fst::SymbolTable *word_syms;
  
  fst::Fst<fst::StdArc> *decode_fst;
  
  kaldi::FasterOnlineDecoder *decoder;
  
  
  float tot_like;
  kaldi::int64 frame_count;
  kaldi::int32 num_success, num_fail;

  kaldi::Timer timer;

  kaldi::int32 utt_frame_count;
  kaldi::int32 utt_number;
  
  std::string utt_key;
  std::string prev_utt_key;
  
};

#endif









