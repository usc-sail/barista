#include "command-line-interface.h"

using namespace std;
using namespace cppa;

behavior CommandLineInterface::Run(){
  return (
    on(atom("START")) >> [=]() {
  		aout << "Usage: press enter to toggle microphone ON/OFF." << endl;
  		aout << "       press Q to exit the program." << endl;
      char c = '\0';
  		while (true) {
        c = cin.get();
        if (c == 'q')
          break;
        switch (c) {
          case '\n':
            aout << " ++  " << name << ": Sending toggle. " << endl;
            send(subscribers, atom("TOGGLE"));
            break;
  			}
  		}
      aout << " ++  " << name << ": Quitting... " << endl;
      self->quit(exit_reason::user_shutdown);
    },
    on(atom("QUIT")) >> [=]() {
      self->quit(exit_reason::user_shutdown);
    }
  );
}
