#ifndef VMAILMGR__DAEMON__COMMAND__H__
#define VMAILMGR__DAEMON__COMMAND__H__

#include "mystring/mystring.h"

class command
{
private:
  const mystring cmd;
  unsigned argc;
  mystring** args;
  command();
  command(const command&);
public:
  command(mystring, unsigned);
  ~command();
  mystring& operator[](unsigned i) { return *args[i]; }
  mystring operator[](unsigned i) const { return *args[i]; }
  mystring name() const { return cmd; }
  unsigned count() const { return argc; }

  bool replace_first_two(mystring);
};
  
#endif // VMAILMGR__DAEMON__COMMAND__H__
