////////////////////////////////////////////////////////////////////////////////
// 
// HashMap.hh
//    produced: 01/09/97 jr
// last change: 24/01/97 jr
//
////////////////////////////////////////////////////////////////////////////////
#ifndef HASHMAP_HH
#define HASHMAP_HH

#include <stdlib.h>
#include <iostream>
#include <ctype.h>
#include <string.h>

#include "RefCount.hh"
#include "PlainHashMap.hh"

namespace topcom {

  template<class Key, class T>	
  class HashMap;

  template<class Key, class T>
  inline std::istream& operator>>(std::istream& ist, HashMap<Key, T>& ht);

  template<class Key, class T>
  inline std::ostream& operator<<(std::ostream& ost, const HashMap<Key, T>& ht);

  template<class Key, class T>	
  class HashMap {
    typedef typename PlainHashMap<Key, T>::dataptr_type  dataptr_type;
    typedef          SmartPtr< PlainHashMap<Key, T> >    data_type;
    typedef typename PlainHashMap<Key, T>::entry         entry_type;
  private:
    static const PlainHashMap<Key, T> default_PlainHashMap;
  private:
    data_type _data;
  public:
    // constructors:
    inline HashMap() : _data(default_PlainHashMap) {}
    inline HashMap(const size_type new_bucket_count) : _data(PlainHashMap<Key, T>(new_bucket_count)) {}
    inline HashMap(const HashMap<Key, T>& ht) : _data(ht._data) {}
    // destructor:
    inline ~HashMap() {}
    // assignment:
    inline HashMap& operator=(const HashMap<Key, T>& ht) { _data = ht._data; return *this; }
    // comparison:
    inline bool operator==(const HashMap<Key, T>& ht) const { 
      return (*_data == *ht._data);
    }
    inline bool operator!=(const HashMap<Key, T>& ht) const {
      return !((*this) == ht); 
    }
    // keys for containers:
    inline const size_type keysize()              const { return _data->keysize(); }
    inline const size_type key(const size_type n) const { return _data->key(n); }
    // accessors:
    inline const int            empty() const { return _data->empty(); }
    inline const size_type      card()     const { return _data->size(); }
    inline const size_type      bucket_count()     const { return _data->bucket_count(); }
    inline const size_type      size()     const { return _data->size(); }
    //  inline const entry_type**   data()     const { return _data->data(); }
#ifdef WATCH_MAXCHAINLEN
    inline static const size_type      maxchainlen() { return PlainHashMap<Key, T>::maxchainlen(); }
#endif
    // operations:
    inline const dataptr_type& member(const Key& key)            const { return _data->member(key); }
    inline       dataptr_type& member(const Key& key)                  { return _data->member(key); }
    inline       dataptr_type& insert(const Key& key, const T& obj)    { return _data->insert(key, obj); }
    inline       dataptr_type& overwrite(const Key& key, const T& obj) { return _data->overwrite(key, obj); }
    inline       void          erase(const Key& key)                   { _data->erase(key); }
    inline       void          erase_random()                          { _data->erase_random(); }
    inline       void          clear()                                 { _data->clear(); }
    // operators:
    inline const T&           operator[](const Key& key) const { return (*_data)[key]; }
    inline       T&           operator[](const Key& key)       { return (*_data)[key]; }
    // iterator types:
    typedef typename PlainHashMap<Key, T>::const_iterator const_iterator;
    typedef typename PlainHashMap<Key, T>::iterator       iterator;
    // iterator functions:
    inline const_iterator begin()              const { return _data->begin();}
    inline const_iterator end()                const { return _data->end();}
    inline const_iterator find(const Key& key) const { return _data->find(key); }
    inline iterator       begin()                    { return _data->begin();}
    inline iterator       end()                      { return _data->end();}
    inline iterator       find(const Key& key)       { return _data->find(key); }

    // iostream:
    inline std::istream& read(std::istream& ist) {
      return ist >> *_data;
    }
    inline std::ostream& write(std::ostream& ost) const {
      return ost << *_data;
    }

    friend std::istream& operator>><>(std::istream& ist, HashMap<Key, T>& ht);
    friend std::ostream& operator<<<>(std::ostream& ost, const HashMap<Key, T>& ht);
  };

  template<class Key, class T>
  const PlainHashMap<Key, T> HashMap<Key, T>::default_PlainHashMap;

  // friends:
  template<class Key, class T>
  inline std::istream& operator>>(std::istream& ist, HashMap<Key, T>& ht) {
    return ht.read(ist);
  }

  template<class Key, class T>
  inline std::ostream& operator<<(std::ostream& ost, const HashMap<Key, T>& ht) {
    return ht.write(ost);
  }

}; // namespace topcom

#endif
// eof HashMap.hh
