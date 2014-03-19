#include "vector-writer.h"

#include <matrix/kaldi-vector.h>

using namespace std;
using namespace kaldi;
using namespace cppa;

behavior VectorWriter::Run(){
  return (
    on<string, Vector<BaseFloat> >() >> [=](string key, Vector<BaseFloat> data) {
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
