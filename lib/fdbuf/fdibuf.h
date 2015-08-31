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

#ifndef FDBUF__FDIBUF__H__
#define FDBUF__FDIBUF__H__

class fdibuf : protected fdbuf
{
public:
  fdibuf(const char* filename, unsigned bufsz = FDBUF_SIZE);
  fdibuf(int fdesc, bool dc = false, unsigned bufsz = FDBUF_SIZE);
  virtual ~fdibuf();
  bool close() { lock(); bool r = fdbuf::close(); unlock(); return r; }
  bool eof() const;
  bool operator!() const ;
  operator bool() const { return !operator!(); }
  virtual bool get(char& ch);
  virtual bool getline(mystring& out, char terminator = '\n');
  virtual bool getnetstring(mystring& out);
  virtual bool read(char*, unsigned);
  virtual bool read_large(char*, unsigned);
  bool read(unsigned char* b, unsigned l) { return read((char*)b, l); }
  bool read(signed char* b, unsigned l) { return read((char*)b, l); }
  unsigned last_count() { return count; }
  bool seek(unsigned o);
  bool seekfwd(unsigned o);
  bool rewind() { return seek(0); }
  unsigned tell() const { return offset-buflength+bufstart; }
  int error_number() const { return errnum; }
protected:
  unsigned count;		// Number of bytes read by last operation
  bool refill();
};

extern fdibuf fin;

#endif // FDBUF__FDIBUF__H__
