#ifndef VMAILMGR__PWENTRY_TABLE__H__
#define VMAILMGR__PWENTRY_TABLE__H__

#include "pwentry.h"

#if 0

#include "hash_table.h"
#include "crc_hash.h"

class pwentry_table : public hash_table<pwentry*,hash_ptr_node<pwentry>,crc_hash>
{
  bool valid;
public:
  pwentry_table();
  ~pwentry_table();
  bool operator!() const
    {
      return !valid;
    }
};

#else

class pwentry_table
{
public:
  bool operator!() const { return false; }
  pwentry* operator[](const mystring&) const;
};

#endif

#endif
