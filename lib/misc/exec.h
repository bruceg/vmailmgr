#ifndef VMAILMGR__EXEC__H__
#define VMAILMGR__EXEC__H__

#include "vpwentry/vpwentry.h"
#include "misc/strlist.h"

int setenv(const char* prefix, const mystring& value);
void setenv(const vpwentry*);

int execute_one(const char* args[]);
int execute(const mystring& name);

#endif // VMAILMGR__EXEC__H__
