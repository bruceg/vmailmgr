#include "mystring.h"
#include <ctype.h>
			
mystring mystring::upper() const
{
  const unsigned length = rep->length;
  char buf[length+1];
  const char* in = rep->buf + length;
  bool changed = false;
  for(char* out = buf+length; out >= buf; in--, out--)
    if(islower(*in)) {
      *out = toupper(*in);
      changed = true;
    }
    else
      *out = *in;
  if(!changed)
    return *this;
  else
    return mystring(buf, length);
}
