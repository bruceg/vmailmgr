#ifndef VMAILMGR__PWCRYPT__H__
#define VMAILMGR__PWCRYPT__H__

#include "mystring/mystring.h"

bool crypt_cmp(const mystring& pass, const mystring& crypt);
const char* pwcrypt(const mystring& pass);

#endif
