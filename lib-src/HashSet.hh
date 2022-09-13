////////////////////////////////////////////////////////////////////////////////
// 
// HashSet.hh
//    produced: 01/09/97 jr
// last change: 10/10/97 jr
//
////////////////////////////////////////////////////////////////////////////////
#ifndef HASHSET_HH
#define HASHSET_HH

#include <stdlib.h>
#include <iostream>
#include <ctype.h>
#include <string.h>

#include "RefCount.hh"
#include "PlainHashSet.hh"

namespace topcom {

  template<class Key>	
  class HashSet;

  template<class Key>
  inline std::istream& operator>>(std::istream& ist, HashSet<Key>& ht);

  template<class Key>
  inline std::ostream& operator<<(std::ostream& ost, const HashSet<Key>& ht);

  template<class Key>	
  class HashSet {
    typedef SmartPtr< PlainHashSet<Key> >    data_type;
  private:
    static const PlainHashSet<Key> default_PlainHashSet;
  private:
    data_type _data;
  public:
    // constructors:
    inline HashSet() : _data(default_PlainHashSet) {}
    inline HashSet(const size_type new_bucket_count) : _data(PlainHashSet<Key>(new_bucket_count)) {}
    inline HashSet(const HashSet<Key>& ht) : _data(ht._data) {}
    // destructor:
    inline ~HashSet() {}
    // assignment:
    inline HashSet& operator=(const HashSet<Key>& ht) { _data = ht._data; return *this; }
    // comparison:
    inline bool operator==(const HashSet<Key>& ht) const {
      return (*_data == *ht._data);
    }
    inline bool operator!=(const HashSet<Key>& ht) const {
      return !((*this) == ht);
    }
    // keys for containers:
    inline const size_type keysize()              const { return _data->keysize(); }
    inline const size_type key(const size_type n) const { return _data->key(n); }
    // accessors:
    inline const bool           empty() const { return _data->empty(); }
    inline const size_type      card()     const { return _data->size(); }
    inline const size_type      bucket_count()     const { return _data->bucket_count(); }
    inline const size_type      size()     const { return _data->size(); }
#ifdef WATCH_MAXCHAINLEN
    inline static const size_type      maxchainlen() { return PlainHashSet<Key>::maxchainlen(); }
#endif
    // operations:
    inline bool member(const Key& key) const { return _data->member(key); }
    inline void insert(const Key& key)       { _data->insert(key); }
    inline void erase(const Key& key)        { _data->erase(key); }
    inline void erase_random()               { _data->erase_random(); }
    inline void clear()                      { _data->clear(); }
    // iterator types:
    typedef typename PlainHashSet<Key>::const_iterator const_iterator;
    typedef const_iterator                    iterator;
    // iterator functions:
    inline const_iterator begin()              const {return _data->begin();}
    inline const_iterator end()                const {return _data->end();}
    inline const_iterator find(const Key& key) const { return _data->find(key); }
    // iostream:
    inline std::istream& read(std::istream& ist) {
      return ist >> *_data;
    }
    inline std::ostream& write(std::ostream& ost) const {
      return ost << *_data;
    }
    friend std::istream& operator>><>(std::istream& ist, HashSet<Key>& ht);
    friend std::ostream& operator<<<>(std::ostream& ost, const HashSet<Key>& ht);
  };

  template<class Key>
  const PlainHashSet<Key> HashSet<Key>::default_PlainHashSet;

  // friends:
  template<class Key>
  inline std::istream& operator>>(std::istream& ist, HashSet<Key>& ht) {
    return ht.read(ist);
  }

  template<class Key>
  inline std::ostream& operator<<(std::ostream& ost, const HashSet<Key>& ht) {
    return ht.write(ost);
  }

}; // namespace topcom

#endif
// eof HashSet.hh
