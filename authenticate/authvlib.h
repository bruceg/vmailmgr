#ifndef VMAILMGR__VMAILMGR_AUTH_LIBRARY__H__
#define VMAILMGR__VMAILMGR_AUTH_LIBRARY__H__

#include "mystring/mystring.h"
#include "misc/pwentry.h"

struct user_data
{
  uid_t uid;
  gid_t gid;
  mystring name;
  mystring home;
  mystring maildir;
  mystring vname;
  
  user_data(pwentry* pw, mystring md, mystring v)
    : uid(pw->uid), gid(pw->gid), name(pw->name), home(pw->home),
      maildir(md), vname(v)
  {
  }
  
  ~user_data() { }

  bool non_virtual() const { return !vname; }
};

// The following routines are defined in authvlib.cc
void set_domain(mystring& name, mystring& domain);
user_data* authenticate(mystring name, mystring pass, mystring domain,
			bool virtual_only = false);
void set_user(const pwentry* pw);

// The following three routines must be defined by the authentication module.
void fail_login(const char* msg);
void fail_baddata(const char* msg);
void fail_temporary(const char* msg);

#endif // VMAILMGR__VMAILMGR_AUTH_LIBRARY__H__
