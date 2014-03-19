#include "matrix-writer.h"

#include <matrix/kaldi-matrix.h>

using namespace std;
using namespace kaldi;
using namespace cppa;

behavior MatrixWriter::Run(){
  return (
    on<string, Matrix<BaseFloat> >() >> [=](string key, Matrix<BaseFloat> data) {
      data.Write(*sink, false);
    },
    on(atom("UTT_END")) >> [=]() {
      *sink << endl;
    },
    on(atom("QUIT")) >> [=]() {
      self->quit(exit_reason::user_shutdown);
    },
    on(atom("DONE")) >> [=]() {
      self->quit(exit_reason::normal);
    }
  );
}
