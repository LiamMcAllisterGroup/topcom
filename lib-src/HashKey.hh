////////////////////////////////////////////////////////////////////////////////
// 
// HashKey.hh
//    produced: 01/09/97 jr
// last change: 24/01/99 jr
//
////////////////////////////////////////////////////////////////////////////////
#ifndef HASHKEY_HH
#define HASHKEY_HH

#include "Global.hh"
#include "Field.hh"

namespace topcom {

  template<class Key>
  class HashKeySize {
  public:
    inline size_type operator()(const Key& key) const { return key.keysize(); }
  };

  template<class Key>
  class HashKey {
  public:
    inline size_type operator()(const Key& key, const size_type n) const { return key.key(n); }
  };

  // for the use in STL vectors/unordered sets/maps:
  template<class Key>
  class Hash {
  public:
    inline size_type operator()(const Key& key) const {
      // use single-byte MurmurHash:
      size_type result(525201411107845655ull);
      const size_type keysize = key.keysize();
      for (size_type i = 0; i < keysize; ++i) {
	// result = (result << 5) - result;
	// result += key.key(i);
	result ^= key.key(i);
	result *= 0x5bd1e9955bd1e995;
	result ^= (result >> 47UL);
      }
      return result;
    }
  };

  template<>
  class Hash<size_type> {
  public:
    inline size_type operator()(const size_type key) const {
      return key;
    }
  };

  template<>
  class Hash<int> {
  public:
    inline size_type operator()(const int key) const {
      return key;
    }
  };

  //////////////////////////////////////////////////////////////////////////////
  // some specializations:
  //////////////////////////////////////////////////////////////////////////////

  // size_type:

  template<>
  class HashKeySize<size_type> {
  public:
    size_type operator()(const size_type& key) const { return 1; }
  };

  template<>
  class HashKey<size_type> {
  public:
    size_type operator()(const size_type& key, const size_type n) const { return key; }
  };

  template<>
  class HashKeySize<Field> {
  public:
    inline size_type operator()(const Field& key) const { return 1; }
  };

  // Field:
  
  template<>
  class HashKey<Field> {
  public:
    inline size_type operator()(const Field& key, const size_type n) const { 
      hash<Field> hash_obj;
      return hash_obj(key); 
    }
  };

}; // namespace topcom

#endif
// eof HashKey.hh
