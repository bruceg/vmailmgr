#ifndef VMAILMGR__LIST_TABLE__H__
#define VMAILMGR__LIST_TABLE__H__

#include "hash_table.h"
#include "crc_hash.h"
#include "mystring.h"

class list_table : public hash_table<bool,hash_node<bool>,crc_hash>
{
  bool valid;
public:
  list_table(const mystring& filename);
  ~list_table();
  bool operator!() const
    {
      return !valid;
    }
};

#endif
