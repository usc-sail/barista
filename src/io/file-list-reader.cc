#include "file-list-reader.h"

using namespace std;
using namespace cppa;

behavior FileListReader::Run(){
  return (
    on(atom("START")) >> [=]() {
      ifstream files(file_list.c_str());
      string file_name;
      while(getline(files, file_name)) {
        if (file_name != "")
          send(subscribers, "", file_name);
      }
      files.close();
      aout << " ++  " << name << ": Finished reading file list: " << file_list << endl;
      send(subscribers, atom("DONE"));
      self->quit(exit_reason::normal);
    },
    on(atom("QUIT")) >> [=]() {
      self->quit(exit_reason::user_shutdown);
    }
  );
}
