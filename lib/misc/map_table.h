#ifndef VMAILMGR__MAP_TABLE__H__
#define VMAILMGR__MAP_TABLE__H__

#include "misc/hash_table.h"
#include "misc/crc_hash.h"
#include "mystring/mystring.h"

class map_table : public hash_table<mystring,hash_node<mystring>,crc_hash>
{
  bool valid;
public:
  map_table(const mystring& filename);
  ~map_table();
  bool operator!() const
    {
      return !valid;
    }
};

#endif
