#include "apply-cmvn.h"

using namespace std;
using namespace kaldi;
using namespace cppa;

/// Discard stats gathered from a single frame (weighted).
void DiscardCmvnStats(const VectorBase<BaseFloat> &feats,
                      BaseFloat weight,
                      MatrixBase<double> *stats) {
  int32 dim = feats.Dim();
  KALDI_ASSERT(stats != NULL);
  KALDI_ASSERT(stats->NumRows() == 2 && stats->NumCols() == dim + 1);
  // Remove these __restrict__ modifiers if they cause compilation problems.
  // It's just an optimization.
  double *__restrict__ mean_ptr  = stats->RowData(0),
         *__restrict__ var_ptr   = stats->RowData(1),
         *__restrict__ count_ptr = mean_ptr + dim;
  const BaseFloat * __restrict__ feats_ptr = feats.Data();
  *count_ptr -= weight;
  // Careful-- if we change the format of the matrix, the "mean_ptr < count_ptr"
  // statement below might become wrong.
  for (; mean_ptr < count_ptr; mean_ptr++, var_ptr++, feats_ptr++) {
    *mean_ptr -= *feats_ptr * weight;
    *var_ptr -=  *feats_ptr * *feats_ptr * weight;
  }                     
}

behavior ApplyCMVN::Run(){
  return (
    on<string, Matrix<BaseFloat> >() >> [=](string key, Matrix<BaseFloat> data) {
      
      if (buffer.NumRows() == 0) {
        buffer.Resize(buffer_size, data.NumCols(), kUndefined);
        out_data.Resize(data.NumRows() + context_size, data.NumCols(), kUndefined);
        InitCmvnStats(data.NumCols(), &cmvn_stats);
        full = false;   // buffer is not full
        head = 0;
        current = 0;
        last = 0;
      }
      
      for (int i = 0; i < data.NumRows(); i++) {
        // The frame at buffer head is not included in the current context 
        // so we discard the cmvn stats gathered from that frame
        if (full)
          DiscardCmvnStats(buffer.Row(head), 1.0, &cmvn_stats);
        // Accumulate the stats gathered from the data frame
        AccCmvnStats(data.Row(i), 1.0, &cmvn_stats);
        // Insert data frame to the buffer at head position
        buffer.CopyRowFromVec(data.Row(i), head);
        // Update buffer head index
        head = (head + 1) % buffer_size;
        if (!full) {
          if (head == 0) {
            last = context_size + 1;
            NormalizeFrames();
            full = true;
          }
        } else {
          last = current + 1;
          NormalizeFrames();
        }
      }
      
      // Send normalized frames
      if (out_size > 0) {
        out_data.Resize(out_size, buffer.NumCols(), kCopyData);
        send(subscribers, key, out_data);
        out_size = 0;
      }
    },
    on(atom("UTT_END")) >> [=]() {
      // Normalize remaning frames of current utterance
      last = head;
      NormalizeFrames();
      if (out_size > 0) {
        out_data.Resize(out_size, buffer.NumCols(), kCopyData);
        send(subscribers, "", out_data);
        out_size = 0;
      }
      send(subscribers, atom("UTT_END"));
      buffer.Resize(0, 0);
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

bool ApplyCMVN::NormalizeFrames() {
  // Compute number of frames to be normalized and update output size
  uint32 num_frames = (last - current + buffer_size) % buffer_size;
  uint32 row_offset = out_size;
  out_size = row_offset + num_frames;
  
  // Resize output matrix if necessary
  if (out_size > out_data.NumRows()) {
    out_data.Resize(out_size, buffer.NumCols(), kCopyData);
  }
  // Copy frames to be normalized into output
  for (int i = row_offset; i < out_size; i++) {
    out_data.CopyRowFromVec(buffer.Row(current), i);
    current = (current + 1) % buffer_size;
  }
  // Apply CMVN
  auto frames = out_data.RowRange(row_offset, num_frames);  
  ApplyCmvn(cmvn_stats, norm_vars, &frames);
  return true;
}
