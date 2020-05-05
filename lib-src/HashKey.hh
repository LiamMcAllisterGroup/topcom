////////////////////////////////////////////////////////////////////////////////
// 
// HashKey.hh
//    produced: 01/09/97 jr
// last change: 24/01/99 jr
//
////////////////////////////////////////////////////////////////////////////////
#ifndef HASHKEY_HH
#define HASHKEY_HH

#include <Global.hh>

template<class Key>
class HashKeySize {
public:
  inline size_type operator()(const Key& key) { return key.keysize(); }
};

template<class Key>
class HashKey {
public:
  inline size_type operator()(const Key& key, const size_type n) { return key.key(n); }
};

//////////////////////////////////////////////////////////////////////////////
// some specializations:
//////////////////////////////////////////////////////////////////////////////

// size_type:

template<>
class HashKeySize<size_type> {
public:
  size_type operator()(const size_type& key) { return 1; }
};

template<>
class HashKey<size_type> {
public:
  size_type operator()(const size_type& key, const size_type n) { return key; }
};

#include <Field.hh>

template<>
class HashKeySize<Field> {
public:
  inline size_type operator()(const Field& key) { return 1; }
};

// Field:

template<>
class HashKey<Field> {
public:
  inline size_type operator()(const Field& key, const size_type n) { 
    static std_ext::hash<Field> hash_obj;
    return hash_obj(key); 
  }
};

#endif
// eof HashKey.hh
