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

#include <config.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "mystring/mystring.h"
#include "fdbuf/fdbuf.h"
#include "configrc.h"

#if 0
configuration::configuration()
  : parent(0)
{
}
#endif

configuration::configuration(const configuration* prev, const mystring& dir)
  : parent(prev), directory(dir)
{
}

configuration::~configuration()
{
}

static configuration global_config(0, GLOBAL_CONFIG_DIR);
const configuration* config = &global_config;
