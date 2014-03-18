#include "portaudio-reader.h"

using namespace std;
using namespace kaldi;
using namespace cppa;

// Defines the running behavior
behavior PaReader::Run(){
  return (
    on(atom("TOGGLE")) >> [=]() {
      if (active) {
        audio_src->Read(&data);
        send(subscribers, "mic", data);
        send(subscribers, atom("UTT_END"));
        aout << endl << " ++  " << name << ": Microphone OFF." << endl;
        active = false;
      } else {
        active = true;
        aout << " ++  " << name << ": Microphone ON. Listening..." << endl;
      }
    },
    on(atom("QUIT")) >> [=]() {
      self->quit(exit_reason::user_shutdown);
    },
    after(std::chrono::milliseconds(200)) >> [=]() {
      if (active) {
        aout << "." << flush;
        audio_src->Read(&data);
        send(subscribers, "mic", data);
      }
    }
  );
}


