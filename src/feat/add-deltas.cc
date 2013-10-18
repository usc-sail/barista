#include "add-deltas.h"

using namespace std;
using namespace kaldi;
using namespace cppa;

behavior AddDeltas::Run(){
  return (
    on<string, Matrix<float> >() >> [=](string key, Matrix<float> feats) {
      
      // redundant check -- this should already be taken care of
      // if (feats.NumRows() == 0) {
      //     continue;
      // }
      
      // Create dummy pad feats for first feature block
      if (pad_feats.NumRows() == 0) {
          pad_feats.Resize(pad_size / 2, feats.NumCols());
          for (MatrixIndexT i = 0; i < pad_size / 2; i++)
              pad_feats.CopyRowFromVec(feats.Row(0), i);
      }
      
      // Create a new matrix for padded features
      Matrix<BaseFloat> padded_feats(pad_feats.NumRows() + feats.NumRows(),
                                     feats.NumCols());
      
      // Add pad features
      padded_feats.Range(0, pad_feats.NumRows(), 
                         0, pad_feats.NumCols()).CopyFromMat(pad_feats);
      // Add current features
      padded_feats.Range(pad_feats.NumRows(), feats.NumRows(), 
                         0, feats.NumCols()).CopyFromMat(feats);
      
      // Compute delta features
      ComputeDeltas(opts, padded_feats, &delta_feats);
      
      // Send truncated delta feats
      if (delta_feats.NumRows() > pad_size) {
          out_data = Matrix<BaseFloat>(
                          SubMatrix<BaseFloat>(delta_feats, pad_size / 2, 
                                               delta_feats.NumRows() - pad_size,
                                               0, delta_feats.NumCols()));
          send(subscribers, key, out_data);
      }

      // Update pad features
      pad_feats = Matrix<BaseFloat>(
                      SubMatrix<BaseFloat>(padded_feats,   
                                           max(0, padded_feats.NumRows() - pad_size), 
                                           min(padded_feats.NumRows(), pad_size), 
                                           0, padded_feats.NumCols()));
    },
    on(atom("UTT_END")) >> [=]() {
      // Guard against a very short utterance
      MatrixIndexT offset = max(pad_size / 2, delta_feats.NumRows() - pad_size / 2);
      out_data = Matrix<BaseFloat>(
                      SubMatrix<BaseFloat>(delta_feats, 
                                           offset, delta_feats.NumRows() - offset, 
                                           0, delta_feats.NumCols()));
      // Send the remaining delta feats
      send(subscribers, "", out_data);
      
      // Empty pad feats
      pad_feats.Resize(0, 0);
      
      // Send the utterance end signal
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

