#ifndef VMAILMGR__SERVER__H__
#define VMAILMGR__SERVER__H__

#include "mystring/mystring.h"
#include "misc/response.h"
#include "config/configrc.h"

class server_call
{
private:
  mystring cmd;
  unsigned argc;
  mystring* args;
  int fd;
  
  server_call();
  server_call(const server_call&);
  
  mystring build_msg() const;
  int connect(const mystring&) const;
  void disconnect();
  
public:
  server_call(mystring, unsigned a = 0);
  server_call(mystring, mystring);
  server_call(mystring, mystring, mystring);
  server_call(mystring, mystring, mystring, mystring);
  server_call(mystring, mystring, mystring, mystring, mystring);
  server_call(mystring, mystring, mystring, mystring, mystring, mystring);
  server_call(mystring, mystring, mystring, mystring, mystring, mystring,
	      mystring);
  ~server_call();
  response call(const mystring& = config->socket_file());
  void operand(int i, mystring m) { args[i] = m; }
  mystring operand(int i) const { return args[i]; }
  int socket() const { return fd; }
};

#endif
