// Copyright (C) 1999,2000,2005 Bruce Guenter <bruce@untroubled.org>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#include <config.h>
#include <unistd.h>
#include <stdlib.h>
#ifdef HAVE_CRYPT_H
#include <crypt.h>
#endif
#include "pwcrypt.h"

#ifndef HAVE_CRYPT
char *crypt(const char *key, const char *salt);
#endif

#ifndef HAVE_RANDOM
long int random(void);
#endif

extern "C"
{
#include "md5.h"
};

static const char bin2hex_table[17] = "0123456789abcdef";

#define OLD_MD5_CODE '3'

static const char* encrypt_old_md5(const mystring& pass)
{
  md5_ctx ctx;
  md5_init_ctx(&ctx);
  md5_process_bytes(pass.c_str(), pass.length(), &ctx);
  unsigned char md5[16];
  md5_finish_ctx(&ctx, md5);
  static char out[34];
  out[0] = OLD_MD5_CODE;
  for(unsigned i = 0; i < 16; i++) {
    out[i*2+1] = bin2hex_table[md5[i]>>4];
    out[i*2+2] = bin2hex_table[md5[i]&0xf];
  }
  out[33] = 0;
  return out;
}

extern "C" char *md5_crypt __P ((const char *key, const char *salt));

const char* null_crypt(const mystring& pass)
{
  static mystring s;
  s = "$0$" + pass;
  return s.c_str();
}

bool crypt_cmp(const mystring& pass, const mystring& stored)
{
  if(!stored || !pass)
    return false;
  const char* encrypted;
  if(stored.length() == 33 && stored[0] == OLD_MD5_CODE)
    encrypted = encrypt_old_md5(pass.c_str());
  else if(stored[0] == '$' && stored[1] == '1' && stored[2] == '$')
    encrypted = md5_crypt(pass.c_str(), stored.c_str());
  else if(stored[0] == '$' && stored[1] == '0' && stored[2] == '$')
    encrypted = null_crypt(pass);
  else
    encrypted = crypt(pass.c_str(), stored.c_str());
  return stored == encrypted;
}

#ifdef USE_CRYPT

static const char passwd_table[65] =
"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789./";

const char* pwcrypt(const mystring& pass)
{
  char salt[2] = {
    passwd_table[random() % 64],
    passwd_table[random() % 64]
  };
  return crypt(pass.c_str(), salt);
}

#else

static const char passwd_table[65] =
"./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

const char* pwcrypt(const mystring& pass)
{
  char salt[14] = "$1$";
  for(unsigned i = 3; i < 11; i++)
    salt[i] = passwd_table[random() % 64];
  salt[12] = 0;
  return md5_crypt(pass.c_str(), salt);
}

#endif
