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

#ifndef HASH_TABLE__H__
#define HASH_TABLE__H__

#include "mystring/mystring.h"

struct hash_base
{
  const mystring key;
  const unsigned hash;
  hash_base(const mystring& k, unsigned h)
    : key(k), hash(h)
    {
    }
};

template<class T>
struct hash_node : public hash_base
{
  T data;
  hash_node(const mystring& k, unsigned h, T d)
    : hash_base(k, h), data(d)
    {
    }
};

template<class T>
struct hash_ptr_node : public hash_base
{
  T* data;
  hash_ptr_node(const mystring& k, unsigned h, T* d)
    : hash_base(k, h), data(d)
    {
    }
  ~hash_ptr_node()
    {
      if(data)
	delete data;
    }
};

static const unsigned hash_prime_list[] = {
  53,         97,         193,       389,       769,
  1543,       3079,       6151,      12289,     24593,
  49157,      98317,      196613,    393241,    786433,
  1572869,    3145739,    6291469,   12582917,  25165843,
  50331653,   100663319,  201326611, 402653189, 805306457,
  1610612741, 3221225473U, 4294967291U
};
  
template<class TYPE, class NODE, class HASH>
class hash_table_iterator;

template<class TYPE, class NODE, class HASH>
class hash_table
{
  friend class hash_table_iterator<TYPE,NODE,HASH>;
  
  TYPE dummy;
  HASH hash;
  NODE** table;
  unsigned prime_index;
  unsigned prime;
  unsigned oversize;
  unsigned _count;

  unsigned rehash(unsigned h) const
    {
      return (h + 1) % prime;
    }
  
  void alloc()
    {
      prime = hash_prime_list[prime_index];
      oversize = prime*3/4;
      table = new NODE*[prime];
      for(unsigned i = 0; i < prime; i++)
	table[i] = 0;
      _count = 0;
    }

  void realloc(unsigned new_index)
    {
      //unsigned old_prime_index = prime_index;
      unsigned old_prime = prime;
      NODE** old_table = table;
      prime_index = new_index;
      alloc();
      for(unsigned i = 0; i < old_prime; i++)
	if(old_table[i])
	  base_insert(old_table[i], true);
    }

  unsigned lookup(const mystring& key, unsigned hash) const
    {
      for(unsigned h = hash % prime; table[h]; h = rehash(h))
	if(table[h]->hash == hash && table[h]->key == key)
	  return h;
      return prime;
    }

  unsigned lookup(const mystring& key) const
    {
      return lookup(key, hash(key));
    }

  bool base_insert(NODE* e, bool unique)
    {
      unsigned h;
      for(h = e->hash % prime; table[h]; h = rehash(h)) {
	if(table[h]->hash == e->hash &&
	   table[h]->key == e->key) {
	  if(unique)
	    return false;
	  else {
	    delete table[h];
	    --_count;
	    break;
	  }
	}
      }
      table[h] = e;
      ++_count;
      return true;
    }
      
public:
  typedef hash_table_iterator<TYPE,NODE,HASH> iterator;
  
  hash_table()
    : dummy(0), prime_index(0)
    {
      alloc();
    }
  
  ~hash_table()
    {
      if(table) {
	empty();
	delete[] table;
      }
    }

  void empty()
    {
      for(unsigned i = 0; i < prime; i++)
	if(table[i]) {
	  delete table[i];
	  table[i] = 0;
	}
      _count = 0;
    }

  bool exists(const mystring& key) const
    {
      return lookup(key) >= prime;
    }

  const TYPE& operator[](const mystring& key) const
    {
      unsigned h = lookup(key);
      return (h >= prime) ? dummy : table[h]->data;
    }
      
  TYPE& operator[](const mystring& key)
    {
      unsigned h = lookup(key);
      return (h >= prime) ? dummy : table[h]->data;
    }
      
  unsigned count() const 
    {
      return _count;
    }
  
  bool insert(const mystring& key, TYPE data)
    {
      NODE* e = new NODE(key, hash(key), data);
      if(_count >= oversize)
	realloc(prime_index+1);
      return base_insert(e, true);
    }
  
  bool set(const mystring& key, TYPE data)
    {
      NODE* e = new NODE(key, hash(key), data);
      if(_count >= oversize)
	realloc(prime_index+1);
      return base_insert(e, false);
    }

  bool remove(const mystring& key, bool mustexist = false)
    {
      unsigned h = lookup(key);
      if(h < prime) {
	delete table[h];
	table[h] = 0;
	--_count;
	return true;
      }
      else
	return !mustexist;
    }
};

// hash table iterator
// usage: for(var.iterator i = var; !i; i++) use(*i);
template<class TYPE, class NODE, class HASH>
class hash_table_iterator
{
  hash_table<TYPE,NODE,HASH>& table;
  unsigned index;
  void next()
    {
      while(index < table.prime && !table.table[index])
	++index;
    }
public:
  hash_table_iterator(hash_table<TYPE,NODE,HASH>& t) : table(t), index(0)
    {
      next();
    }
  bool operator!() const
    {
      return index < table.prime;
    }
  bool at_end() const
    {
      return index >= table.prime;
    }
  void operator++() 
    {
      ++index;
      next();
    }
  TYPE& operator*()
    {
      return table.table[index]->data;
    }
  const TYPE& operator*() const
    {
      return table.table[index]->data;
    }
};

class hash_sample
{
public:
  unsigned operator()(const mystring& str) const
    {
      unsigned h = 0;
      for(unsigned i = 0; i < str.length(); i++)
	h = (h << 1) ^ (unsigned)str[i];
      return h;
    }
};

#endif
