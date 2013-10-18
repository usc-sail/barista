#include "module-base.h"

using namespace std;
using namespace cppa;

behavior ModuleBase::Config(){
  return (
    on(atom("JOIN"), arg_match) >> [=](const group_ptr& gptr) {
      self->join(gptr);
      return atom("JOINED");
    },
    on(atom("LINK"), arg_match) >> [=](const actor_ptr& other) {
      self->link_to(other);
      return atom("LINKED");
    },
    on(atom("QUIT")) >> [=]() {
      self->quit(exit_reason::user_shutdown);
    },
    on(atom("RUN")) >> [=]() {
      become(running_state);
    }
  );
}
