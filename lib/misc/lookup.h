#ifndef VMAILMGR__LOOKUP__H__
#define VMAILMGR__LOOKUP__H__

#include "misc/pwentry.h"

extern bool is_local(mystring);
extern mystring find_virtual(mystring);
extern bool lookup_baseuser(mystring fulluser, pwentry* &pw, mystring& virtname);

#endif // VMAILMGR__LOOKUP__H__
