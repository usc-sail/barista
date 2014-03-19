#include "filter.h"

#include <matrix/kaldi-vector.h>

using namespace std;
using namespace kaldi;
using namespace cppa;

behavior Filter::Run(){
  return (
    on<string, Vector<BaseFloat> >() >> [=](string key, Vector<BaseFloat> data) {
      for (int i = 0; i < data.Dim(); i++) {
        data(i) = data(i) * weight;
      }
      send(subscribers, key, data);
    },
    on(atom("UTT_END")) >> [=]() {
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

