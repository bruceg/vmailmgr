// Copyright (C) 1999,2000 Bruce Guenter <bruce@untroubled.org>
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
#include "vdomain.h"
#include "misc/crc_hash.h"

mystring vdomain::userdir(mystring username) const
{
  unsigned slices = config.user_dir_slices();
  unsigned bits = config.user_dir_bits();
  static crc_hash hasher;
  static const char bin2hex[16+1] = "0123456789abcdef";
  unsigned hash = hasher(username);
  mystring dir = config.user_dir();
  const unsigned hexdigits = (bits+3)/4;
  const unsigned bitmask = ~(~0U << bits);
  char hexbuf[hexdigits+1];
  hexbuf[hexdigits] = 0;
  for(; slices > 0; --slices, hash >>= bits) {
    unsigned slice = hash & bitmask;
    for(unsigned i = hexdigits; i > 0; --i, slice >>= 4)
      hexbuf[i-1] = bin2hex[slice & 0xf];
    dir += hexbuf;
    dir += "/";
  }
  dir += username.subst('.', ':');
  if(!!subdir)
    dir = subdir + "/" + dir;
  return dir;
}
