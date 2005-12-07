// Copyright (C) 2000 Bruce Guenter <bruce@untroubled.org>
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
#include <ctype.h>
#include <string.h>
#include "vpwentry.h"

bool vpwentry::from_ver2_record(const mystring& text)
{
  // Sanity check to ensure that text conversions can't
  // fall off the end of the string.
  if(text[text.length()-1] != 0)
    return false;

  const char* ptr = text.c_str();
  const char* const end = ptr++ + text.length();

  if((ptr = decode_flags(ptr, end)) == 0) return false;
  if((ptr = decode_base(ptr, end)) == 0) return false;
  if((ptr = decode_values(ptr, end)) == 0) return false;
  
  //data = ptr;
  
  return ptr == end;
}
