////////////////////////////////////////////////////////////////////////////////
// 
// PlainHashIndexTable.hh
//    produced: 06/04/98 jr
// last change: 06/04/98 jr
//
////////////////////////////////////////////////////////////////////////////////
#ifndef PLAINHASHINDEXTABLE_HH
#define PLAINHASHINDEXTABLE_HH

#include <stdlib.h>
#include <iostream>
#include <ctype.h>
#include <string.h>

#include "Global.hh"
#include "RefCount.hh"
#include "Array.hh"
#include "PlainHashMap.hh"

template<class Key>
class PlainHashIndexTable : public PlainHashMap<Key, size_type> {
public:
  typedef          PlainHashMap<Key, size_type>                     plainhashindextable_data;
  typedef typename PlainHashMap<Key, size_type>::keyptr_type        keyptr_type;
  typedef typename PlainHashMap<Key, size_type>::const_keyptr_type  const_keyptr_type;
  typedef typename PlainHashMap<Key, size_type>::dataptr_type       dataptr_type;
  typedef typename PlainHashMap<Key, size_type>::const_dataptr_type const_dataptr_type;
  typedef          Array<const_keyptr_type>                         index_data_type;
private:
  index_data_type _index_data;
public:
  // constructors:
  inline PlainHashIndexTable() : plainhashindextable_data() {}
  inline PlainHashIndexTable(const plainhashindextable_data& phitd) : plainhashindextable_data(phitd) {}
  inline PlainHashIndexTable(const PlainHashIndexTable& phit) : plainhashindextable_data(phit) {}
  inline PlainHashIndexTable(const size_type init_size) : plainhashindextable_data(init_size) {}
  // destructor:
  inline ~PlainHashIndexTable() {}
  // assignment:
  inline PlainHashIndexTable& operator=(const PlainHashIndexTable& phit) {
    plainhashindextable_data::operator=(phit);
    return *this;
  }
  // accessors:
  inline const size_type maxindex() const { return _index_data.maxindex(); }
  // functions:
  inline const Key& get_obj(const size_type index) const {
#ifdef INDEX_CHECK
    if (index >= _index_data.maxindex()) {
      std::cerr << "Key& get_obj(const size_type): "
	   << "index out of range." << std::endl;
    }
#endif
   return *_index_data[index];
 }
  inline const size_type get_index(const Key& key) {
    const dataptr_type& indexptr(this->member(key));
    if (indexptr) {
      return *indexptr;
    }    
    const size_type new_index(_index_data.maxindex());
    const HashMapData<Key, size_type>* 
      hashptr(plainhashindextable_data::plainhashmap_data::insert(HashMapData<Key, size_type>(key, new_index)));
    _index_data.resize(new_index + 1);
    _index_data[new_index] = hashptr->keyptr();
    return new_index;
  }
  inline void insert(const Key& key) { get_index(key); }
private:
  void erase(const Key&);		// no erase allowed!
  size_type operator[](const Key&);	// no non-const reference to Data allowed!
};

#endif
// eof PlainHashIndexTable.hh
