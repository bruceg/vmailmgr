#ifndef VMAILMGR__VDOMAIN__H__
#define VMAILMGR__VDOMAIN__H__

#include "mystring/mystring.h"
#include "config/configrc.h"
#include "misc/pwentry.h"
#include "vpwtable/vpwtable.h"
#include "misc/response.h"

class vdomain
{
public:
  const mystring subdir;
  const mystring prefix;
  const configuration config;
private:
  vpwtable* ptable;

  // Helper functions for chattr
  response chpass(vpwentry*, mystring);
  response chdest(vpwentry*, mystring);
public:
  vdomain(const pwentry&);
  vdomain(const pwentry&, mystring, mystring, const configuration&);
  ~vdomain();

  vpwtable* table();
  
  mystring userdir(mystring username) const;

  // Virtual user manipulation functions
  vpwentry* lookup(mystring name, bool nodefault);
  bool exists(mystring name);
  
  response set(const vpwentry*, bool onlyadd, mystring maildir = "");
  response chattr(mystring user, unsigned attr, mystring newval);
  response chattr(const vpwentry*, unsigned attr, mystring newval);
  response deluser(mystring name);

  // Virtual user validation functions
  response validate_forward(mystring);
  bool validate_username(mystring) const;
  bool validate_password(mystring) const;

  static const unsigned ATTR_PASS = 1;
  static const unsigned ATTR_DEST = 2;
  static const unsigned ATTR_HARDQUOTA = 3;
  static const unsigned ATTR_SOFTQUOTA = 4;
  static const unsigned ATTR_MSGSIZE = 5;
  static const unsigned ATTR_MSGCOUNT = 6;
  static const unsigned ATTR_EXPIRY = 7;
  static const unsigned ATTR_MAILBOX_ENABLED = 8;
  static const unsigned ATTR_PERSONAL = 9;
};

#endif // VMAILMGR__VDOMAIN__H__
