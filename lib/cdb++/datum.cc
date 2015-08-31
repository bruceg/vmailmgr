// Copyright (C) 1999,2000,2005 Bruce Guenter <bruce@untroubled.org>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA

#include "datum.h"

datum::datum(const mystring& k, const mystring& d)
  : key(k), data(d)
{
}

datum::~datum()
{
}

fdobuf& operator<<(fdobuf& out, const datum& d)
{
  return out << '+' << d.key.length() << ',' << d.data.length()
	     << ':' << d.key << "->" << d.data;
}
