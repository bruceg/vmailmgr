// Copyright (C) 1999,2000 Bruce Guenter <bruceg@em.ca>
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

#ifndef VMAILMGR__DEBUG__H__
#define VMAILMGR__DEBUG__H__

#define trace(X) trace2(this,X)
#define traceptr(X,P) trace3(this,X " " #P "=",P)
#define tracestr(X,P) trace4(this,X " " #P "=(",P)

#ifdef TRACE

#include <unistd.h>

static inline const char* itoh(unsigned i)
{
  static char convert[17] = "0123456789abcdef";
  static char buf[9];
  for(unsigned j = 8; j > 0; ) {
    --j;
    buf[j] = convert[i & 0xf];
    i >>= 4;
  }
  buf[8] = 0;
  return buf;
}

static inline void trace2(const void* s, const char* x)
{
  write(2, itoh((unsigned)s), 8);
  write(2, ": ", 2);
  write(2, x, strlen(x));
  write(2, "\n", 1);
}

static inline void trace3(const void* s, const char* x, const void* p)
{
  write(2, itoh((unsigned)s), 8);
  write(2, ": ", 2);
  write(2, x, strlen(x));
  write(2, itoh((unsigned)p), 8);
  write(2, "\n", 1);
}

static inline void trace4(const void* s, const char* x, const char* p)
{
  write(2, itoh((unsigned)s), 8);
  write(2, ": ", 2);
  write(2, x, strlen(x));
  write(2, itoh((unsigned)p), 8);
  write(2, ")'", 2);
  write(2, p, strlen(p));
  write(2, "'\n", 2);
}

#else
#define trace2(X,P)
#define trace3(T,X,P)
#define trace4(T,X,P)
#endif

#ifdef DEBUG

static inline void debug(const char* x)
{
  write(2, x, strlen(x));
  write(2, "\n", 1);
}

static inline void fail(int code, const char* message)
{
  debug(message);
  exit(code);
}

#else
#define fail(X,Y) exit(X)
#define debug(X)
#endif

#endif
