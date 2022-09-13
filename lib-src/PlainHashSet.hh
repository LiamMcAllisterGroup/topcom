////////////////////////////////////////////////////////////////////////////////
// 
// PlainHashSet.hh
//    produced: 01/09/97 jr
// last change: 24/01/99 jr
//
////////////////////////////////////////////////////////////////////////////////
#ifndef PLAINHASHSET_HH
#define PLAINHASHSET_HH

#include <stdlib.h>
#include <iostream>
#include <ctype.h>
#include <string.h>

#include "Global.hh"
#include "RefCount.hh"
#include "PlainHashTable.hh"

namespace topcom {

  template<class Key>
  class HashSetData;

  template<class Key>
  class __hsd_reader;

  template<class Key>
  inline std::istream& operator>>(std::istream& ist, __hsd_reader<Key>& r);

  template<class Key>
  class __hsd_reader {
  public:
    Key  key;
  public:
    inline __hsd_reader() : key() {}
    friend std::istream& operator>><>(std::istream& ist, __hsd_reader& r);
  };

  template<class Key>
  inline std::ostream& operator<<(std::ostream& ost, const HashSetData<Key>& hsd);

  template<class Key>
  class HashSetData {
  public:
    typedef Key                  key_type;
    typedef SmartPtr<Key>        keyptr_type;
    typedef SmartPtr<const Key>  const_keyptr_type;
    typedef __hsd_reader<Key>    reader;
  private:
    const const_keyptr_type _keyptr;
  public:
    // constructors:
    inline HashSetData() : _keyptr() {}
    inline HashSetData(const HashSetData& hsd) : _keyptr(hsd._keyptr) {}
    inline HashSetData(const Key& new_key) : _keyptr(new_key) {}
    inline HashSetData(const reader& r) : _keyptr(r.key) {}
    // destructor:
    inline ~HashSetData() {}
  private:
    // assignment:
    inline HashSetData& operator=(const HashSetData& hsd) {
      if (this == &hsd) {
	return *this;
      }
      _keyptr = hsd._keyptr;
      return *this;
    }
  public:
    // conversion:
    inline operator const Key&() const { return *_keyptr; }
    // keys for containers:
    inline const keyptr_type& keyptr()  const { return _keyptr; }
    inline const Key&         key()     const { return *_keyptr; }
    // comparison:
    inline const bool operator==(const HashSetData<Key>& hsd) const { return (key() == hsd.key()); }
    inline const bool operator!=(const HashSetData<Key>& hsd) const { return !(key() == hsd.key()); }
    // iostream:
    inline std::ostream& write(std::ostream& ost) const {
      return ost << *_keyptr;
    }
    friend std::ostream& operator<<<>(std::ostream& ost, const HashSetData<Key>& hsd);
  };

  template<class Key>
  class PlainHashSet : public PlainHashTable< HashSetData<Key> > {
  public:
    typedef          PlainHashTable< HashSetData<Key> >       plainhashset_data;
    typedef typename HashSetData<Key>::keyptr_type            keyptr_type;
    typedef typename HashSetData<Key>::const_keyptr_type      const_keyptr_type;
  public:
    // constructors:
    inline PlainHashSet() : plainhashset_data() {}
    inline PlainHashSet(const PlainHashSet& phs) : plainhashset_data(phs) {}
    inline PlainHashSet(const plainhashset_data& phsd) : plainhashset_data(phsd) {}
    inline PlainHashSet(const size_type init_bucket_count) : plainhashset_data(init_bucket_count) {}
    // destructor:
    inline ~PlainHashSet() {}
    // assignment:
    inline PlainHashSet& operator=(const PlainHashSet& phs) {
      plainhashset_data::operator=(phs);
      return *this;
    }
    // functions:
    inline void insert(const Key& key)       { plainhashset_data::insert(key); }
    inline bool member(const Key& key) const { return plainhashset_data::member(key); }
  };

  template<class Key>
  inline std::istream& operator>>(std::istream& ist, __hsd_reader<Key>& r) {
    if (!(ist >> std::ws >> r.key)) {
#ifdef READ_DEBUG
      std::cerr << "std::istream& operator>>(std::istream&, HashMapData<Key, Data>&): "
		<< "key not found." << std::endl;
#endif
      ist.clear(std::ios::failbit);
      return ist;
    }
    return ist;
  }

  template<class Key>
  inline std::ostream& operator<<(std::ostream& ost, const HashSetData<Key>& hsd) {
    return hsd.write(ost);
  }

}; // namespace topcom

#endif
// eof PlainHashSet.hh
