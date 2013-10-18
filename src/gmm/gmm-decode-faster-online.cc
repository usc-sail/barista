#include "gmm-decode-faster-online.h"

using namespace std;
using namespace kaldi;
using namespace cppa;


fst::Fst<fst::StdArc>* ReadNetwork(std::string& filename) {
    // read decoding network FST
    kaldi::Input ki(filename); // use ki.Stream() instead of is.
    if (!ki.Stream().good()) KALDI_ERR << "Could not open decoding-graph FST "
        << filename;    
    fst::FstHeader hdr;
    if (!hdr.Read(ki.Stream(), "<unknown>")) {
        KALDI_ERR << "Reading FST: error reading FST header.";
    }
    if (hdr.ArcType() != fst::StdArc::Type()) {
        KALDI_ERR << "FST with arc type " << hdr.ArcType() << " not supported.\n";
    }
    fst::FstReadOptions ropts("<unspecified>", &hdr);
    fst::Fst<fst::StdArc> *decode_fst = NULL;
    if (hdr.FstType() == "vector") {
        decode_fst = fst::VectorFst<fst::StdArc>::Read(ki.Stream(), ropts);
    } else if (hdr.FstType() == "const") {
        decode_fst = fst::ConstFst<fst::StdArc>::Read(ki.Stream(), ropts);
    } else {
        KALDI_ERR << "Reading FST: unsupported FST type: " << hdr.FstType();
    }
    if (decode_fst == NULL) { // fst code will warn.
        KALDI_ERR << "Error reading FST (after reading header).";
        return NULL;
    } else {
        return decode_fst;
    }
}

void WriteBestPath(FasterOnlineDecoder &decoder,
                   bool allow_partial,
                   BaseFloat acoustic_scale,
                   std::string &key,
                   std::string &lattice_wspecifier,
                   int32 utt_frame_count,
                   fst::SymbolTable *word_syms,                  
                   Int32VectorWriter &words_writer,
                   Int32VectorWriter &alignment_writer,
                   CompactLatticeWriter &clat_writer,
                   int *num_success,
                   int *num_fail,
                   kaldi::int64 *frame_count,
                   BaseFloat *tot_like) {
    
    fst::VectorFst<LatticeArc> decoded;  // linear FST.
    
    if ( (allow_partial || decoder.ReachedFinal())
        && decoder.GetBestPath(&decoded) ) {
        if (!decoder.ReachedFinal())
            KALDI_WARN << "Decoder did not reach end-state, "
            << "outputting partial traceback since --allow-partial=true";
        *num_success += 1;
        if (!decoder.ReachedFinal())
            KALDI_WARN << "Decoder did not reach end-state, outputting partial traceback.";
        std::vector<int32> alignment;
        std::vector<int32> words;
        LatticeWeight weight;
        *frame_count += utt_frame_count;
        
        GetLinearSymbolSequence(decoded, &alignment, &words, &weight);
        
        if (words_writer.IsOpen())
            words_writer.Write(key, words);
        if (alignment_writer.IsOpen())
            alignment_writer.Write(key, alignment);
        
        if (lattice_wspecifier != "") {
            if (acoustic_scale != 0.0) // We'll write the lattice without acoustic scaling
                fst::ScaleLattice(fst::AcousticLatticeScale(1.0 / acoustic_scale), &decoded);
            fst::VectorFst<CompactLatticeArc> clat;
            ConvertLattice(decoded, &clat, true);
            clat_writer.Write(key, clat);
        }
        
        if (word_syms != NULL) {
            std::cerr << key << ' ';
            for (size_t i = 0; i < words.size(); i++) {
                std::string s = word_syms->Find(words[i]);
                if (s == "")
                    KALDI_ERR << "Word-id " << words[i] <<" not in symbol table.";
                std::cerr << s << ' ';
            }
            std::cerr << '\n';
        }
        BaseFloat like = -weight.Value1() -weight.Value2();
        *tot_like += like;
        KALDI_LOG << "Log-like per frame for utterance " << key << " is "
        << (like / utt_frame_count) << " over "
        << utt_frame_count << " frames.";
        KALDI_VLOG(2) << "Cost for utterance " << key << " is "
        << weight.Value1() << " + " << weight.Value2();
    } else {
        *num_fail += 1;
        KALDI_WARN << "Did not successfully decode utterance " << key
        << ", len = " << utt_frame_count;
    }
}

behavior GMMDecodeFasterOnline::Run(){
  return (
    on<string, Matrix<float> >() >> [=](string key, Matrix<float> feats) {

      // read key
      std::stringstream ss;
      ss << utt_number;
      utt_key = ss.str();
      
      DecodableAmDiagGmmScaled gmm_decodable(am_gmm, trans_model, feats,
                                             acoustic_scale);
      
      // If utt_key has changed, initialize decoder for the new utterance
      if (utt_key.compare(prev_utt_key) != 0) {
          decoder->InitDecode();
      }
      decoder->Decode(&gmm_decodable);
      utt_frame_count += feats.NumRows();
      prev_utt_key = utt_key;
    },
    on(atom("UTT_END")) >> [=]() {
      // finish decoding previous utterance
      if (utt_frame_count > 0) {
          WriteBestPath(*decoder, allow_partial, acoustic_scale, utt_key, 
                        lattice_wspecifier, utt_frame_count, word_syms,
                        words_writer, alignment_writer, clat_writer, 
                        &num_success, &num_fail, &frame_count, &tot_like);
          utt_frame_count = 0;
      }
      utt_number++;
    },
    on(atom("QUIT")) >> [=]() {
      self->quit(exit_reason::user_shutdown);
    },
    on(atom("DONE")) >> [=]() {
      double elapsed = timer.Elapsed();
      KALDI_LOG << "Time taken [excluding initialization] "<< elapsed
      << "s: real-time factor assuming 100 frames/sec is "
      << (elapsed*100.0/frame_count);
      KALDI_LOG << "Done " << num_success << " utterances, failed for "
      << num_fail;
      KALDI_LOG << "Overall log-likelihood per frame is " 
      << (tot_like/frame_count) << " over " << frame_count<<" frames.";
      self->quit(exit_reason::normal);
    }
  );
}
