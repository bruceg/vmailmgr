#ifndef CRC_HASH__H__
#define CRC_HASH__H__

#include "misc/crc32.h"

class crc_hash
{
public:
  unsigned operator()(const mystring& key) const
    {
      crc32_c crc;
      crc.update(key.c_str(), key.length());
      return crc;
    }
};

#endif
