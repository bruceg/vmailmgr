#ifndef VMAILMGR__MAILDIR__H__
#define VMAILMGR__MAILDIR__H__

#include <sys/types.h>
#include "mystring/mystring.h"

int mkdirp(const mystring&, mode_t);
bool make_maildir(const mystring&);
bool delete_maildir(const mystring&);

#endif
