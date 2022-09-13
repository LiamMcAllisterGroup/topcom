////////////////////////////////////////////////////////////////////////////////
// 
// PlainHashMap.hh
//    produced: 01/09/97 jr
// last change: 24/01/99 jr
//
////////////////////////////////////////////////////////////////////////////////
#ifndef PLAINHASHMAP_HH
#define PLAINHASHMAP_HH

#include <stdlib.h>
#include <iostream>
#include <ctype.h>
#include <string.h>

#include "Global.hh"
#include "RefCount.hh"
#include "PlainHashTable.hh"

namespace topcom {

  template<class Key, class Data>
  class HashMapData;

  template<class Key, class Data>
  class __hmd_reader;

  template<class Key, class Data>
  inline std::istream& operator>>(std::istream& ist, __hmd_reader<Key, Data>& r);

  template<class Key, class Data>
  inline std::ostream& operator<<(std::ostream& ost, const HashMapData<Key, Data>& hmd);

  template<class Key, class Data>
  class __hmd_reader {
  public:
    Key  key;
    Data data;
  public:
    friend std::istream& operator>><>(std::istream& ist, __hmd_reader& r);
  };

  template<class Key, class Data>
  class HashMapData {
  public:
    typedef Key                      key_type;
    typedef Data                     data_type;
    typedef SmartPtr<Key>            keyptr_type;
    typedef SmartPtr<const Key>      const_keyptr_type;
    typedef SmartPtr<Data>           dataptr_type;
    typedef SmartPtr<const Data>     const_dataptr_type;
    typedef __hmd_reader<Key, Data>  reader;
  private:
    const const_keyptr_type  _keyptr;
    dataptr_type             _dataptr;
  public:
    static data_type    default_data;
    static dataptr_type null;
  public:
    // constructors:
    inline HashMapData() : _keyptr(), _dataptr() {}
    inline HashMapData(const HashMapData& hmd) : _keyptr(hmd._keyptr), _dataptr(hmd._dataptr) {}
    inline HashMapData(const Key& new_key, const Data& new_data) : _keyptr(new_key), _dataptr(new_data) {}
    inline HashMapData(const reader& r) : _keyptr(r.key), _dataptr(r.data) {}
    // destructor:
    inline ~HashMapData() {}
  private:
    // assignment:
    inline HashMapData& operator=(const HashMapData& hmd) {
      if (this == &hmd) {
	return *this;
      }
      (keyptr_type)_keyptr = hmd._keyptr;
      _dataptr = hmd._dataptr;
      return *this;
    }
  public:
    // accessors:
    inline const const_keyptr_type&  keyptr()  const { return _keyptr; }
    inline const dataptr_type&       dataptr() const { return _dataptr; }
    inline       dataptr_type&       dataptr()       { return _dataptr; }
    inline const Key&                key()     const { return *_keyptr; }
    inline const Data&               data()    const { return *_dataptr; }
    inline       Data&               data()          { return *_dataptr; }
    // comparison:
    inline const bool operator==(const HashMapData<Key, Data>& hmd) const { 
      return (key() == hmd.key()) && (data() == hmd.data()); 
    }
    inline const bool operator!=(const HashMapData<Key, Data>& hmd) const { 
      return !((key() == hmd.key()) && (data() == hmd.data())); 
    }
    // iostream:
    inline std::ostream& write(std::ostream& ost) const {
      return ost << *_keyptr << "->" << *_dataptr;
    }
    friend std::ostream& operator<<<>(std::ostream& ost, const HashMapData<Key, Data>& hmd);
  };

  template<class Key, class Data>
  class PlainHashMap : public PlainHashTable< HashMapData<Key, Data> > {
  public:
    typedef          PlainHashTable< HashMapData<Key, Data> >   plainhashmap_data;
    typedef typename HashMapData<Key, Data>::keyptr_type        keyptr_type;
    typedef typename HashMapData<Key, Data>::const_keyptr_type  const_keyptr_type;
    typedef typename HashMapData<Key, Data>::dataptr_type       dataptr_type;
    typedef typename HashMapData<Key, Data>::const_dataptr_type const_dataptr_type;
  public:
    // constructors:
    inline PlainHashMap() : plainhashmap_data() {}
    inline PlainHashMap(const PlainHashMap& phm) : plainhashmap_data(phm) {}
    inline PlainHashMap(const plainhashmap_data& phmd) : plainhashmap_data(phmd) {}
    inline PlainHashMap(const size_type init_bucket_count) : plainhashmap_data(init_bucket_count) {}
    // destructor:
    inline ~PlainHashMap() {}
    // assignment:
    inline PlainHashMap& operator=(const PlainHashMap& phm) {
      plainhashmap_data::operator=(phm);
      return *this;
    }
    // functions:
    inline const dataptr_type& member(const Key& key) const {
      const HashMapData<Key, Data>* hashptr(plainhashmap_data::member(key));
      if (hashptr) return hashptr->dataptr();
      else return HashMapData<Key, Data>::null;
    }
    inline dataptr_type& member(const Key& key) {
      HashMapData<Key, Data>* hashptr(plainhashmap_data::member(key));
      if (hashptr) return hashptr->dataptr();
      else return HashMapData<Key, Data>::null;
    }
    inline dataptr_type& insert(const Key& new_key, const Data& new_data) {
      return plainhashmap_data::insert(HashMapData<Key, Data>(new_key, new_data))->dataptr();
    }
    inline dataptr_type& overwrite(const Key& new_key, const Data& new_data) {
      dataptr_type& dataptr = 
	plainhashmap_data::insert(HashMapData<Key, Data>(new_key, HashMapData<Key, Data>::default_data))->dataptr();
      *dataptr = new_data;
      return dataptr;
    }
    inline const Data& operator[](const Key& key) const { 
      return *member(key); 
    }
    inline Data& operator[](const Key& key) {
      return *insert(key, HashMapData<Key, Data>::default_data); 
    }
  };
 
  template<class Key, class Data>
  typename HashMapData<Key, Data>::data_type HashMapData<Key, Data>::default_data;
 
  template<class Key, class Data>
  typename HashMapData<Key, Data>::dataptr_type HashMapData<Key, Data>::null;

  // friends:

  template<class Key, class Data>
  inline std::istream& operator>>(std::istream& ist, __hmd_reader<Key, Data>& r) {
    char dash;
    char arrow;
    if (!(ist >> std::ws >> r.key)) {
#ifdef READ_DEBUG
      std::cerr << "std::istream& operator>>(std::istream&, HashMapData<Key, Data>::reader&): "
		<< "key not found." << std::endl;
#endif
      ist.clear(std::ios::failbit);
      return ist;
    }
    if (!(ist >> std::ws >> dash >> arrow)) {
#ifdef READ_DEBUG
      std::cerr << "std::istream& operator>>(std::istream&, HashMapData<Key, Data>::reader&): "
		<< "`->' not found." << std::endl;
#endif
      ist.clear(std::ios::failbit);
      return ist;
    }
    if (!(ist >> std::ws >> r.data)) {
#ifdef READ_DEBUG
      std::cerr << "std::istream& operator>>(std::istream&, HashMapData<Key, Data>::reader&): "
		<< "data not found." << std::endl;
#endif
      ist.clear(std::ios::failbit);
      return ist;
    }
    return ist;
  }

  template<class Key, class Data>
  inline std::ostream& operator<<(std::ostream& ost, const HashMapData<Key, Data>& hmd) {
    return hmd.write(ost);
  }

}; // namespace topcom

#endif
// eof PlainHashMap.hh
