#ifndef VMAILMGR__RESPONSE__H__
#define VMAILMGR__RESPONSE__H__

#include "mystring/mystring.h"

struct response
{
  enum response_code { ok=0, bad=1, err=2, econn=3 };
  const response_code code;
  const mystring msg;

  response(response_code c, mystring m) : code(c), msg(m) { }
  response(const response& r) : code(r.code), msg(r.msg) { }
  ~response() { }

  static response read(int fd);
  bool write(int fd) const;
  bool operator!() const { return code != ok; }
  mystring message() const;
  mystring codestr() const;

  static const unsigned long maxsize = 1<<16-1;
};

#define RETURN(CODE,STR) return response(response::CODE, STR)

#endif
