#ifndef VMAILMGR__CONFIG_IO__H__
#define VMAILMGR__CONFIG_IO__H__

#include "mystring/mystring.h"
#include "config/configrc.h"

bool config_read(const mystring& dir, const mystring& name, mystring& result);

#define cval(TYPE,FN,VAL,EXT) static TYPE default_##FN = VAL; \
static mystring filename_##FN(#FN); \
TYPE configuration:: FN () const { return read_##EXT (filename_##FN, default_##FN); }
#define cval_str(FN,VAL) cval(mystring,FN,VAL,str)
#define cval_dir(FN,VAL) cval(mystring,FN,VAL,dir)
#define cval_uns(FN,VAL) cval(unsigned,FN,VAL,uns)

#endif
