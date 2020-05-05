////////////////////////////////////////////////////////////////////////////////
// 
// PlainHashTable.hh
//    produced: 01/09/97 jr
// last change: 10/10/97 jr
//
////////////////////////////////////////////////////////////////////////////////
#ifndef PLAINHASHTABLE_HH
#define PLAINHASHTABLE_HH

#include <stdlib.h>
#include <iostream>
#include <ctype.h>
#include <string.h>

#include <memory>

#include "Global.hh"
#include "RefCount.hh"
#include "HashKey.hh"

#ifndef _RAND_SEED
#define _RAND_SEED 19345
#endif

// the following prime list is taken from STL, restricted to doubling:
const size_type __pht_no_of_primes = 66;

const size_type __pht_prime[__pht_no_of_primes] = {
  0UL,          3UL,          7UL,         13UL,        27UL,
  53UL,         97UL,         193UL,       389UL,       769UL,
  1543UL,       3079UL,       6151UL,      12289UL,     24593UL,
  49157UL,      98317UL,      196613UL,    393241UL,    786433UL,
  1572869UL,    3145739UL,    6291469UL,   12582917UL,  25165843UL,
  50331653UL,   100663319UL,  201326611UL, 402653189UL, 805306457UL, 
  1610612741UL, 3221225473UL, 4294967291UL,
  // Trick taken from STL:
  // Sentinel, so we don't have to test the result of lower_bound,
  // or, on 64-bit machines, rest of the table.
  sizeof(unsigned long) != 8 ? 4294967291ul : (unsigned long)6442450933ull,
  (unsigned long)12884901857ull,
  (unsigned long)25769803693ull,
  (unsigned long)51539607367ull,
  (unsigned long)103079215087ull,
  (unsigned long)206158430123ull,
  (unsigned long)412316860387ull,
  (unsigned long)824633720731ull,
  (unsigned long)1649267441579ull,
  (unsigned long)3298534883309ull,
  (unsigned long)6597069766607ull,
  (unsigned long)13194139533241ull,
  (unsigned long)26388279066581ull,
  (unsigned long)52776558133177ull,
  (unsigned long)105553116266399ull,
  (unsigned long)211106232532861ull,
  (unsigned long)562949953421231ull,
  (unsigned long)1125899906842597ull,
  (unsigned long)2251799813685119ull,
  (unsigned long)4503599627370449ull,
  (unsigned long)9007199254740881ull,
  (unsigned long)18014398509481951ull,
  (unsigned long)36028797018963913ull,
  (unsigned long)72057594037927931ull,
  (unsigned long)144115188075855859ull,
  (unsigned long)288230376151711717ull,
  (unsigned long)576460752303423433ull,
  (unsigned long)1152921504606846883ull,
  (unsigned long)2305843009213693951ull,
  (unsigned long)4611686018427387847ull,
  (unsigned long)9223372036854775783ull,
  (unsigned long)18446744073709551557ull,
  (unsigned long)18446744073709551557ull
};

template<class HashData>
class __pht_entry {
public:
  HashData*           hashptr;
  __pht_entry*        next;
private:
  __pht_entry() {}
  __pht_entry(const __pht_entry&) {}
public:
  inline __pht_entry(HashData* new_hashptr, __pht_entry* new_next) :
    hashptr(new_hashptr), next(new_next) {}
  inline ~__pht_entry() {}
};

template<class HashData>
class PlainHashTable;

template<class HashData>
class __pht_iterator;

template<class HashData>
class __pht_const_iterator;

template <class HashData>
inline std::istream& operator>>(std::istream& ist, PlainHashTable<HashData>& p);

template <class HashData>
inline std::ostream& operator<<(std::ostream& ost, const PlainHashTable<HashData>& p);

template<class HashData>
class PlainHashTable {
public:
  typedef __pht_entry<HashData> entry;
public:
  std::allocator<HashData>                               HashData_allocator;
  std::allocator<entry*>                                 entryptr_allocator;
  std::allocator<entry>                                  entry_allocator;
  std::allocator<size_type>                              size_type_allocator;
  typedef typename HashData::const_keyptr_type           const_keyptr_type;
  typedef typename HashData::keyptr_type                 keyptr_type;
  typedef typename HashData::key_type                    key_type;
private:
  static HashKeySize<key_type> _hashkeysize;
  static HashKey<key_type>     _hashkey;
private:
  enum {_init_prime = 0UL, _init_maxkey = 0UL, _maxloadfactor = 70UL,
	_large_prime = sizeof(unsigned long) != 8 ? 3221225473U : 18446744073709551557ull,
	_small_prime = 1610612741UL,
	_rand_seed = _RAND_SEED };
#ifdef WATCH_MAXCHAINLEN
private:
  static size_type _maxchainlen;
public:
  static const size_type maxchainlen() { return _maxchainlen; }
#endif
private:
  static entry*              _bufptr;
private:
  entry**    _data;		// data of the PlainHashTable
  size_type  _current_prime;	// current index of prime used as size
  size_type  _size;		// actual PlainHashTable size
  size_type  _load;		// number of elements in the table
  size_type* _randvec;		// coefficients of the hash function
  size_type  _maxkey;		// maximal number of keys per object
private:
  inline void _init_data(entry**, const size_type);
  void        _init_randvec(size_type*, const size_type);
  void        _term_data(entry**, const size_type);
  inline void _term_randvec(size_type*, const size_type);
  void        _copy_data(entry**, const entry*const*, const size_type);
  inline void _copy_randvec(size_type*, const size_type*, const size_type);
  void        _extend_randvec(const size_type new_maxkey);
  inline void _expand();
  inline void _contract();
  void        _rehash(const size_type new_size);
private:
  inline const size_type _hash(const key_type& key) const;
public:
  // constructors:
  inline PlainHashTable();
  PlainHashTable(const size_type); // create PlainHashTable of particular size
  inline PlainHashTable(const PlainHashTable&);
  // destructor:
  inline ~PlainHashTable();
  // assignment:
  PlainHashTable& operator=(const PlainHashTable& ht);
  // comparison:
  bool operator==(const PlainHashTable&) const;
  inline bool operator!=(const PlainHashTable&) const;
  // keys for containers:
  inline const size_type keysize()              const;
  const        size_type key(const size_type n) const;
  // accessors:
  inline const bool          is_empty() const {return (_load == 0);}
  inline const size_type     card()     const {return _load;}
  inline const size_type     size()     const {return _size;}
  inline const size_type     load()     const {return _load;}
  inline const entry**       data()     const {return _data;}
  // functions:
  const HashData*     member(const key_type&) const; // needs ``=='' for key_type
  HashData*           member(const key_type&); // needs ``=='' for key_type
  HashData*           insert(const HashData&); // inserts a HashData if key not already there
  void                erase(const key_type&); // deletes a key
  void                erase_random(); // deletes a random key 
				      // (for cache functionality)
  void                clear();	// deletes all keys
  // stream output/input:
  std::istream& read(std::istream&);
  std::ostream& write(std::ostream&) const;
  friend std::istream& operator>><>(std::istream& ist, PlainHashTable<HashData>& p);
  friend std::ostream& operator<<<>(std::ostream& ost, const PlainHashTable<HashData>& p);
  // iterator:
  friend class __pht_iterator<HashData>;
  friend class __pht_const_iterator<HashData>;
  typedef __pht_iterator<HashData>       iterator;
  typedef __pht_const_iterator<HashData> const_iterator;
  // iterator functions:
  inline const_iterator begin()               const  {return const_iterator(*this);}
  inline const_iterator end()                 const  {return const_iterator(*this,0);}
  inline const_iterator find(const key_type&) const;
  inline iterator       begin()                      {return iterator(*this);}
  inline iterator       end()                        {return iterator(*this,0);}
  inline iterator       find(const key_type&);
};


template <class HashData>
class __pht_iterator {
  typedef typename PlainHashTable<HashData>::key_type key_type;
private:
  PlainHashTable<HashData>*                     _container;
  size_type                                     _current_index;
  typename PlainHashTable<HashData>::entry*     _current_entryptr;
private:
  __pht_iterator() {}
public:
  // constructors:
  __pht_iterator(PlainHashTable<HashData>&);
  inline __pht_iterator(PlainHashTable<HashData>&, int);
  inline __pht_iterator(const __pht_iterator&);
  __pht_iterator(PlainHashTable<HashData>&, const size_type);
  __pht_iterator(PlainHashTable<HashData>&, const key_type&);
  // destructor:
  inline ~__pht_iterator();
  // assignment:
  inline __pht_iterator operator=(const __pht_iterator& iter);
  // accessors:
  inline const PlainHashTable<HashData>*                 container()        const;
  inline const size_type                                 current_index()    const;
  inline const typename PlainHashTable<HashData>::entry* current_entryptr() const;
  // functions:
  inline bool       operator==(const __pht_iterator&) const;
  inline bool       operator!=(const __pht_iterator&) const;
  
  inline HashData&        operator*() const;
  inline HashData*        operator->() const;
  inline __pht_iterator&  operator++();
  inline __pht_iterator   operator++(int);
};

template <class HashData>
class __pht_const_iterator {
  typedef typename PlainHashTable<HashData>::key_type key_type;
private:
  const PlainHashTable<HashData>*                     _container;
  size_type                                           _current_index;
  const typename PlainHashTable<HashData>::entry*     _current_entryptr;
private:
  __pht_const_iterator() {}
public:
  // constructors:
  __pht_const_iterator(const PlainHashTable<HashData>&);
  inline __pht_const_iterator(const PlainHashTable<HashData>&, int);
  inline __pht_const_iterator(const __pht_const_iterator&);
  inline __pht_const_iterator(const __pht_iterator<HashData>&);
  __pht_const_iterator(const PlainHashTable<HashData>&, const size_type);
  __pht_const_iterator(const PlainHashTable<HashData>&, const key_type&);
  // destructor:
  inline ~__pht_const_iterator();
  // assignment:
  inline __pht_const_iterator operator=(const __pht_const_iterator& iter);
  // accessors:
  inline const PlainHashTable<HashData>*                 container()        const;
  inline const size_type                                 current_index()    const;
  inline const typename PlainHashTable<HashData>::entry* current_entryptr() const;
  // functions:
  inline bool                      operator==(const __pht_const_iterator&) const;
  inline bool                      operator!=(const __pht_const_iterator&) const;
  
  inline const  HashData&            operator*() const;
  inline const  HashData*            operator->() const;
  inline __pht_const_iterator&       operator++();
  inline        __pht_const_iterator operator++(int);
};

template<class HashData>
inline void PlainHashTable<HashData>::_init_data(entry** init_data, const size_type init_size) {
  memset(init_data, 0, init_size * sizeof(entry*));
}

template<class HashData>
void PlainHashTable<HashData>::_init_randvec(size_type* init_randvec, const size_type init_maxkey) {
  srand(_rand_seed);
#ifdef CHECK_RANDVEC
    std::cerr << "_init_randvec:" << std::endl;
#endif
  for (size_type i = 0; i < init_maxkey; ++i) {
    while ((init_randvec[i] = random()) == 0UL);
#ifdef CHECK_RANDVEC
    std::cerr << init_randvec[i] << std::endl;
#endif
  }
}

template<class HashData>
void PlainHashTable<HashData>::_term_data(entry** term_data, const size_type term_size) {
  for (size_type i = 0; i < term_size; ++i) {
    const entry* eptr = term_data[i];
    while (eptr) {
      const entry* delptr = eptr;
      eptr = eptr->next;
      HashData_allocator.destroy(delptr->hashptr);
      HashData_allocator.deallocate(delptr->hashptr, 1);
      entry_allocator.deallocate((entry*)delptr, 1);
    }
  }
  entryptr_allocator.deallocate(term_data, term_size);
}

template<class HashData>
inline void PlainHashTable<HashData>::_term_randvec(size_type* term_randvec, const size_type term_size) {
  size_type_allocator.deallocate(term_randvec, term_size);
}

template<class HashData>
void PlainHashTable<HashData>::_copy_data(entry** to_data, 
					  const entry*const* from_data,
					  const size_type from_size) {
  if (to_data == from_data) {
    return;
  }
  for (size_type j = 0; j < from_size; ++j) {
    to_data[j] = 0;
    const entry* eptr = from_data[j];
    while (eptr) {
      HashData* hashptr = HashData_allocator.allocate(1);
      HashData_allocator.construct(hashptr, *eptr->hashptr);
      _bufptr = to_data[j];
      to_data[j] = entry_allocator.allocate(1);
      to_data[j]->hashptr = hashptr;
      to_data[j]->next    = _bufptr;
      eptr = eptr->next;
    }
  }
}

template<class HashData>
inline void PlainHashTable<HashData>::_copy_randvec(size_type* to_randvec,
						    const size_type* from_randvec,
						    const size_type from_maxkey) {
  if (to_randvec == from_randvec) {
    return;
  }
  memcpy(to_randvec, from_randvec, from_maxkey * sizeof(size_type));
#ifdef CHECK_RANDVEC
  for (size_type i = 0; i < from_maxkey; ++i) {
    std::cerr << to_randvec[i] << std::endl;
  }
#endif
}

template<class HashData>
void PlainHashTable<HashData>::_extend_randvec(const size_type new_maxkey) {
#ifdef DEBUG
  std::cout << "_extend_randvec()." << std::endl;
#endif
#ifdef DEBUG
  if (new_maxkey <= _maxkey) {
    std::cerr << "_extend_randvec(const size_type): "
	 << "new_maxkey <= _maxkey." << std::endl;
  }
#endif
  if (new_maxkey == _maxkey) {
    return;
  }
  if (new_maxkey == 0) {
    if (_randvec) {
      size_type_allocator.deallocate(_randvec, _maxkey);
      _randvec = 0;
    }
    _maxkey = new_maxkey;
    return;
  }
  size_type* rp = size_type_allocator.allocate(new_maxkey);
  if (_maxkey > 0) {
    _copy_randvec(rp, _randvec, _maxkey);
  }
  for (size_type i = _maxkey; i < new_maxkey; ++i) {
    while ((rp[i] = random()) == 0UL);
  }
  if (_randvec) {
    size_type_allocator.deallocate(_randvec, _maxkey);
  }
  _randvec = rp;
  _maxkey = new_maxkey;
#ifdef CHECK_RANDVEC
  std::cerr << "_randvec after extension:" << std::endl;
  for (size_type i = 0; i < _maxkey; ++i) {
    std::cerr << _randvec[i] << std::endl;
  }
#endif
}
 
template<class HashData>
inline void PlainHashTable<HashData>::_expand() {
  // if (_current_prime < __pht_no_of_primes - 1) {
  //   _rehash(__pht_prime[++_current_prime]);
  // }
  if (__pht_prime[_current_prime] == __pht_prime[_current_prime+1]) {
    return;
  }
  else {
    _rehash(__pht_prime[++_current_prime]);
  }
}

template<class HashData>
inline void PlainHashTable<HashData>::_contract() {
  if ( (_current_prime > 1) || ((_current_prime > 0) && (_load == 0)) ) {
    _rehash(__pht_prime[--_current_prime]);
  }
}

template<class HashData>
void PlainHashTable<HashData>::_rehash(const size_type new_size) {
#ifdef REHASH_DEBUG
  std::cout << "PlainHashTable::_rehash(" << new_size << "); old_size = " << _size << std::endl;
#endif
  if (new_size == _size) {
    return;
  }
  if (_size == 0) {
    _size = new_size;
    _data = entryptr_allocator.allocate(_size);
    for (size_type i = 0; i < _size; ++i) {
      _data[i] = 0;
    }
    return;
  }
  size_type old_size = _size;
  _size = new_size;				// _hash needs correct _size for correct hashing
  if (_size == 0) {
    if (_data) {
      entryptr_allocator.deallocate(_data, old_size);
      _data = 0;
    }
    return;
  }
  entry** dp = entryptr_allocator.allocate(_size);
  for (size_type i = 0; i < _size; ++i) {
    dp[i] = 0;
  }
  for (size_type i = 0; i < old_size; ++i) {
    entry* eptr = _data[i];
    while (eptr) {
      size_type hash_index = _hash(eptr->hashptr->key()); // new index
      _data[i] = eptr->next;	// unlink first element
      eptr->next = dp[hash_index]; // set next to first element at new hash_index
      dp[hash_index] = eptr;	// link as first element at new hash_index
      eptr = _data[i];
    }
  }
  entryptr_allocator.deallocate(_data, old_size);
  _data = dp;
}

template<class HashData>
inline const size_type PlainHashTable<HashData>::_hash(const key_type& key) const {
  size_type res(0);
  for (size_type i = 0; i < _hashkeysize(key); ++i) {
    //    res = (res + ((_randvec[i] % _size) ^ _hashkey(key, i))) % _size;
    //    res += _randvec[i] * _hashkey(key, i);
    //    res += _randvec[i] ^ _hashkey(key, i);
    res ^= _hashkey(key, i);
  }
  return res % _size;
}

// constructors:
template<class HashData>
inline PlainHashTable<HashData>::PlainHashTable() : 
  _data(0), 
    _current_prime(_init_prime), 
    _size(__pht_prime[_init_prime]), 
    _load(0),
    _randvec(0), 
    _maxkey(_init_maxkey) {
  if (_size > 0) {
    _data = entryptr_allocator.allocate(_size);
    _init_data(_data, _size);
  }
  if (_maxkey > 0) {
    _randvec = size_type_allocator.allocate(_maxkey);
    _init_randvec(_randvec, _maxkey);
  }
#ifdef CHECK_RANDVEC
  std::cerr << "_randvec after default constructor:" << std::endl;
  for (size_type i = 0; i < _maxkey; ++i) {
    std::cerr << _randvec[i] << std::endl;
  }
#endif
}

template<class HashData>
PlainHashTable<HashData>::PlainHashTable(const size_type new_size) :
  _data(0), 
  _current_prime(_init_prime), 
  _load(0),
  _randvec(0), 
  _maxkey(_init_maxkey) {
  while (__pht_prime[_current_prime] < new_size) {
    ++_current_prime;
  }
  _size = __pht_prime[_current_prime];
  if (_size > 0) {
    _data = entryptr_allocator.allocate(_size);
    _init_data(_data, _size);
  }
  if (_maxkey > 0) {
    _randvec = size_type_allocator.allocate(_maxkey);
    _init_randvec(_randvec, _maxkey);
  }
#ifdef CHECK_RANDVEC
  std::cerr << "_randvec after size constructor:" << std::endl;
  for (size_type i = 0; i < _maxkey; ++i) {
    std::cerr << _randvec[i] << std::endl;
  }
#endif
}

template<class HashData>
inline PlainHashTable<HashData>::PlainHashTable(const PlainHashTable<HashData>& ht) :
  _data(0), 
  _current_prime(ht._current_prime),
  _size(ht._size),
  _load (ht._load),
  _randvec(0),
  _maxkey(ht._maxkey) {
  if (_size > 0) {
    _data = entryptr_allocator.allocate(_size);
    _copy_data(_data, ht._data, _size);
  }
  if (_maxkey > 0) {
    _randvec = size_type_allocator.allocate(_maxkey);
    _copy_randvec(_randvec, ht._randvec, _maxkey);
  }
#ifdef CHECK_RANDVEC
  std::cerr << "_randvec after copy constructor:" << std::endl;
  for (size_type i = 0; i < _maxkey; ++i) {
    std::cerr << _randvec[i] << std::endl;
  }
#endif
}

// destructor:
template<class HashData>
inline PlainHashTable<HashData>::~PlainHashTable() {
  if (_randvec) {
    _term_randvec(_randvec, _maxkey);
  }
  if (_data) {
    _term_data(_data, _size);
  }
}

// assignment:
template<class HashData>
PlainHashTable<HashData>& PlainHashTable<HashData>::operator=(const PlainHashTable<HashData>& ht) {
  if (this == &ht) {
    return *this;
  }
  // destroy old table:
  if (_data) {
    _term_data(_data, _size);
  }
  if (_randvec) {
    _term_randvec(_randvec, _maxkey);
  }
  // copy table:
  _current_prime = ht._current_prime;
  _size = ht._size;
  _load = ht._load;
  _maxkey = ht._maxkey;
  if (_size > 0) {
    _data = entryptr_allocator.allocate(_size);
    _copy_data(_data, ht._data, _size);
  }
  else {
    _data = 0;
  }
  if (_maxkey > 0) {
    _randvec = size_type_allocator.allocate(_maxkey);
    _copy_randvec(_randvec, ht._randvec, _maxkey);
  }
  else {
    _randvec = 0;
  }
  return *this;
}

// comparison:
template<class HashData>
bool PlainHashTable<HashData>::operator==(const PlainHashTable<HashData>& ht) const {
  if (load() != ht.load()) {
#ifdef DEBUG
    std::cerr << load() << " (load) not equal to " << ht.load() << std::endl;
    std::cerr << "thus " << *this << " not equal to " << ht << std::endl;
#endif
   return false;
  }
  for (typename PlainHashTable<HashData>::const_iterator iter = this->begin(); 
       iter != this->end(); 
       ++iter) {
    const typename PlainHashTable<HashData>::const_iterator finder(ht.find(iter->key()));
    if (finder != ht.end()) {
      if (*finder != *iter) {
#ifdef DEBUG
	std::cerr << *finder << " not equal to " << *iter << std::endl;
	std::cerr << "thus " << *this << " not equal to " << ht << std::endl;
#endif
	return false;
      }
    }
    else {
#ifdef DEBUG
      std::cerr << iter->key() << " not contained in " << ht << std::endl;
      std::cerr << "thus " << *this << " not equal to " << ht << std::endl;
#endif
      return false;
    }
  }
  return true;
}

template<class HashData>
inline bool PlainHashTable<HashData>::operator!=(const PlainHashTable<HashData>& ht) const {
  return !((*this) == ht);
}

// keys for containers:
template<class HashData>
inline const size_type PlainHashTable<HashData>::keysize() const {
  return 1;
}
template<class HashData>
const size_type PlainHashTable<HashData>::key(const size_type n) const {
  size_type result(0);
  for (const_iterator iter = begin(); iter != end(); ++iter) {
    for (size_type i = 0; i < _hashkeysize(iter->key()); ++i) {
      const size_type hashkey = _hashkey(iter->key(), i);
      result += (hashkey + (hashkey + _small_prime) * hashkey) % _large_prime;
    }
  }
  return result;
}

// functions:
template<class HashData>
const HashData* PlainHashTable<HashData>::member(const key_type& key) const {
  if (_size == 0) {
    return 0;
  }
  if (_maxkey < _hashkeysize(key)) {
    return 0;
  }
#ifdef HASH_DEBUG
  std::cout << "hash_index(" << key << ") = " << _hash(key) << std::endl;
#endif
  const entry* eptr = _data[_hash(key)];  
  while (eptr) {
    if (eptr->hashptr->key() == key) {
#ifdef MEMBER_DEBUG
      std::cout << key << " contained in const " << *this << std::endl;
#endif
      return eptr->hashptr;
    }
    eptr = eptr->next;
  }
  return 0;
}

template<class HashData>
HashData* PlainHashTable<HashData>::member(const key_type& key) {
  if (_size == 0) {
    return 0;
  }
  if (_maxkey < _hashkeysize(key)) {
    return 0;
  }
#ifdef HASH_DEBUG
  std::cout << "hash_index(" << key << ") = " << _hash(key) << std::endl;
#endif
  const entry* eptr = _data[_hash(key)];  
  while (eptr) {
    if (eptr->hashptr->key() == key) {
#ifdef MEMBER_DEBUG
      std::cout << key << " contained in " << *this << std::endl;
#endif
      return eptr->hashptr;
    }
    eptr = eptr->next;
  }
  return 0;
}

template<class HashData>
HashData* PlainHashTable<HashData>::insert(const HashData& hd) {
  if (_size == 0) {
    _expand();
  }
  if (_maxkey < _hashkeysize(hd.key())) {
    _extend_randvec(_hashkeysize(hd.key()));
  }
  size_type hash_index = _hash(hd.key());
#ifdef HASH_DEBUG
  std::cout << "hash_index(" << hd.key() << ") = " << hash_index << std::endl;
#endif
  entry* eptr = _data[hash_index];
#ifdef WATCH_MAXCHAINLEN
  size_type chain_len = 1;
#endif
  while (eptr) {
    if (eptr->hashptr->key() == hd.key()) {
#ifdef UNIQUENESS_DEBUG
      std::cout << hd.key() << " already in " << *this << std::endl;
#endif
      return eptr->hashptr; // already there
    }
    eptr = eptr->next;
#ifdef WATCH_MAXCHAINLEN
    ++chain_len;
#endif
  }
#ifdef VERBOSE_CHAINLEN
  if (chain_len > 0) {
    std::cout << "PlainHashTable<HashData>::insert(const key_type&): chaining" 
	 << std::endl;
    std::cout << "at hash_index: " << hash_index << ", _size: " << _size 
	 << ", chain length: " << chain_len << std::endl;
  }

#endif
  if (_load * 100 / _size > _maxloadfactor) {
    _expand();
  }
  hash_index = _hash(hd.key());
#ifdef UNIQUENESS_DEBUG
  std::cout << hd.key() << " newly inserted in " << *this << std::endl;
#endif
  HashData* hashptr = HashData_allocator.allocate(1);
  HashData_allocator.construct(hashptr, hd);
  _bufptr = _data[hash_index];
  _data[hash_index] = entry_allocator.allocate(1);
  _data[hash_index]->hashptr = hashptr;
  _data[hash_index]->next = _bufptr;
  ++_load;
#ifdef WATCH_MAXCHAINLEN
  if (chain_len > _maxchainlen) {
    _maxchainlen = chain_len;
  }
#endif
  return _data[hash_index]->hashptr;
}

template<class HashData>
void PlainHashTable<HashData>::erase(const key_type& key) {
  if (_size == 0) {
    return;
  }
  if (_maxkey < _hashkeysize(key)) {
    return;
  }
  size_type hash_index = _hash(key);
#ifdef HASH_DEBUG
  std::cout << "hash_index(" << key << ") = " << hash_index << std::endl;
#endif
  entry* eptr = _data[hash_index];
  if (!eptr) {
    return;
  }
  // delete first element at hash_index:
  if (eptr->hashptr->key() == key) {
    entry* delptr = _data[hash_index];
    _data[hash_index] = eptr->next;
    HashData_allocator.destroy(delptr->hashptr);
    HashData_allocator.deallocate(delptr->hashptr, 1);
    entry_allocator.deallocate(delptr, 1);
    --_load;
    if ((100 * _load > __pht_prime[_init_prime] * _maxloadfactor) && (_load * 200 / _size < _maxloadfactor)) {
      _contract();
    }
    return;
  }
  // delete element in chain at hash_index:
  while (eptr->next) {
    entry* delptr = eptr->next;
    if (eptr->next->hashptr->key() == key) {
      eptr->next = eptr->next->next;
      HashData_allocator.destroy(delptr->hashptr);
      HashData_allocator.deallocate(delptr->hashptr, 1);
      entry_allocator.deallocate(delptr, 1);
      --_load;
      if ((100 * _load > __pht_prime[_init_prime] * _maxloadfactor) 
	  && (_load * 200 / _size < _maxloadfactor)) {
	_contract();
      }
      return;
    }
    eptr = eptr->next;
  }  
}

template<class HashData>
void PlainHashTable<HashData>::erase_random() {
  entry* eptr;
  size_type random_index(random() % _size);
  eptr = _data[random_index];
  while (!eptr) {
    random_index = (random_index + 1) % _size;
    eptr = _data[random_index];
  }
  erase(eptr->hashptr->key());
}

template<class HashData>
void PlainHashTable<HashData>::clear() {
#ifdef REHASH_DEBUG
  std::cout << "PlainHashTable<HashData>::clear()" << std::endl;
#endif
  if (_size == 0) {
    return;
  }
  // destroy old table:
  if (_data) {
    _term_data(_data, _size);
  }
  if (_randvec) {
    _term_randvec(_randvec, _maxkey);
  }
  // default construction:
  _current_prime = _init_prime;
  _size = __pht_prime[_init_prime];
  _load = 0;
  _maxkey = _init_maxkey;
  if (_size > 0) {
    _data = entryptr_allocator.allocate(_size);
    _init_data(_data, _size);
  }
  else {
    _data = 0;
  }
  if (_maxkey > 0) {
    _randvec = size_type_allocator.allocate(_maxkey);
    _init_randvec(_randvec, _maxkey);
  }
  else {
    _randvec = 0;
  }
}

// stream input:
template<class HashData>
std::istream& PlainHashTable<HashData>::read(std::istream& ist) {
  char c;
  typename HashData::reader r;

  clear();
  ist >> std::ws >> c;
  if (c == '[') {
    while (ist >> std::ws >> c) {
      if (c == ']') {
	break;
      }
      if (c == ',') {
	continue;
      }
      ist.putback(c);
      if (ist >> r) {
	insert(HashData(r));
      }
      else {
#ifdef READ_DEBUG
	std::cerr << "PlainHashTable<HashData>::read(std::istream&):"
	     << "cannot find hash data." << std::endl;
#endif
	ist.clear(std::ios::failbit);
	return ist;
      }
    }
  }
  else {
#ifdef READ_DEBUG
    std::cerr << "PlainHashTable<HashData>::read(std::istream&):"
	 << "missing `" << '[' << "'." << std::endl;
#endif
    ist.clear(std::ios::failbit);
    return ist;
  }
  ist.clear(std::ios::goodbit);
  return ist;
}

// stream output:
template<class HashData>
std::ostream& PlainHashTable<HashData>::write(std::ostream& ost) const {
  size_type count = 0;

  ost << "[";
  for (typename PlainHashTable<HashData>::const_iterator iter = this->begin(); iter != this->end(); ++iter) {
    ost << *iter;
    count++;
    if (count < _load)
      ost << ",";
  }
  ost << "]";
  return ost;
}

// inlines for PlainHashTable<HashData>::iterator:

// constructors:
template<class HashData>
__pht_iterator<HashData>::__pht_iterator(PlainHashTable<HashData>& ht) : 
  _current_index(0) {
  _container = &ht;
  while ((_current_index < _container->_size) && 
	 !_container->_data[_current_index]) {
    ++_current_index;
  }
  if (_current_index == _container->_size) {
    _current_entryptr = 0;
  }
  else {
    _current_entryptr = _container->_data[_current_index];
  }
}
template<class HashData>
inline  __pht_iterator<HashData>::__pht_iterator(PlainHashTable<HashData>& ht, int) {
  _container = &ht;
  _current_index = _container->_size;
  _current_entryptr = 0;
}
template<class HashData>
inline __pht_iterator<HashData>::__pht_iterator(const __pht_iterator& iter) {
  _container = iter._container;
  _current_index = iter._current_index;
  _current_entryptr = iter._current_entryptr;
}
template<class HashData>
__pht_iterator<HashData>::__pht_iterator(PlainHashTable<HashData>& ht, const size_type index) {
  _container = &ht;
  if (index < _container->_size) {
    _current_index = index;
    _current_entryptr = _container->_data[_current_index];
  }
  else {
    _current_index = _container->_size;
    _current_entryptr = 0;
  }
}
template<class HashData>
__pht_iterator<HashData>::__pht_iterator(PlainHashTable<HashData>& ht, const key_type& key) {
  _container = &ht;
#ifdef CHECK_FIND
  std::cerr << "iterator at " << key << " in " << *_container << std::endl;
#endif
  if ( (_container->_size != 0)
       && (_container->_maxkey >= PlainHashTable<HashData>::_hashkeysize(key))
       && (!_container->is_empty()) ) {
    _current_index = _container->_hash(key);
    _current_entryptr = _container->_data[_current_index];
    while (_current_entryptr) {
      if (_current_entryptr->hashptr->key() == key) {
#ifdef CHECK_FIND
	std::cerr << "true" << std::endl;
#endif
	return;
      }
      _current_entryptr = _current_entryptr->next;
    }
  }
  _current_index = _container->_size;
  _current_entryptr = 0;  
#ifdef CHECK_FIND
  std::cerr << "false" << std::endl;
#endif
}

// destructor:
template<class HashData>
inline __pht_iterator<HashData>::~__pht_iterator() {}

// assignment:
template<class HashData>
inline  __pht_iterator<HashData> __pht_iterator<HashData>::operator=(const __pht_iterator& iter) {
  if (this == &iter) {
    return *this;
  }
  _container = iter._container;
  _current_index = iter._current_index;
  _current_entryptr = iter._current_entryptr;
  return *this;
}

// accessors:
template<class HashData>
inline const PlainHashTable<HashData>* __pht_iterator<HashData>::container() const {
  return _container; 
}
template<class HashData>
inline const size_type __pht_iterator<HashData>::current_index() const {
  return _current_index;
}
template<class HashData>
inline const typename PlainHashTable<HashData>::entry* __pht_iterator<HashData>::current_entryptr() const {
  return _current_entryptr;
}

// functions:
template<class HashData>
inline bool __pht_iterator<HashData>::operator==(const __pht_iterator& iter)
  const {
    return (_current_entryptr == iter._current_entryptr);
  }
template<class HashData>
inline bool __pht_iterator<HashData>::operator!=(const __pht_iterator& iter)
  const {
    return !((*this) == iter);
  }

template<class HashData>
inline HashData& __pht_iterator<HashData>::operator*() const{
  return *_current_entryptr->hashptr;
}
template<class HashData>
inline HashData* __pht_iterator<HashData>::operator->() const {
  return _current_entryptr->hashptr;
}

template<class HashData>
inline __pht_iterator<HashData>& __pht_iterator<HashData>::operator++() {
  if ((_current_entryptr = _current_entryptr->next) != 0) {
    return *this;
  }
  while (++_current_index < _container->_size) {
    if ((_current_entryptr = _container->_data[_current_index]) != 0) {
      break;
    }
  }
  if (_current_index >= _container->_size) {
    _current_entryptr = 0;
  }
  return *this;
}
template<class HashData>
inline __pht_iterator<HashData> __pht_iterator<HashData>::operator++(int) {
  __pht_iterator tmp(*this);
  ++(*this);
  return tmp;
}

// inlines for PlainHashTable<HashData>::const_iterator:

// constructors:
template<class HashData>
__pht_const_iterator<HashData>::__pht_const_iterator(const PlainHashTable<HashData>& ht) : 
  _current_index(0) {
  _container = &ht;
  while ((_current_index < _container->_size) && 
	 !_container->_data[_current_index]) {
    ++_current_index;
  }
  if (_current_index == _container->_size) {
    _current_entryptr = 0;
  }
  else {
    _current_entryptr = _container->_data[_current_index];
  }
}
template<class HashData>
inline __pht_const_iterator<HashData>::__pht_const_iterator(const PlainHashTable<HashData>& ht, int) {
  _container = &ht;
  _current_index = _container->_size;
  _current_entryptr = 0;
}
template<class HashData>
inline __pht_const_iterator<HashData>::__pht_const_iterator(const __pht_const_iterator& iter) {
  _container = iter._container;
  _current_index = iter._current_index;
  _current_entryptr = iter._current_entryptr;
}
template<class HashData>
inline __pht_const_iterator<HashData>::__pht_const_iterator(const __pht_iterator<HashData>& iter) {
  _container = iter.container();
  _current_index = iter.current_index();
  _current_entryptr = iter.current_entryptr();
}
template<class HashData>
__pht_const_iterator<HashData>::__pht_const_iterator(const PlainHashTable<HashData>& ht, const size_type index) {
  _container = &ht;
  if (index < _container->_size) {
    _current_index = index;
    _current_entryptr = _container->_data[_current_index];
  }
  else {
    _current_index = _container->_size;
    _current_entryptr = 0;
  }
}
template<class HashData>
__pht_const_iterator<HashData>::__pht_const_iterator(const PlainHashTable<HashData>& ht, const key_type& key) {
  _container = &ht;
#ifdef CHECK_FIND
  std::cerr << "const iterator at " << key << " in " << *_container << std::endl;
#endif
  if ( (_container->_size != 0)
       && (_container->_maxkey >= PlainHashTable<HashData>::_hashkeysize(key))
       && (!_container->is_empty()) ) {
    _current_index = _container->_hash(key);
    _current_entryptr = _container->_data[_current_index];
    while (_current_entryptr) {
      if (_current_entryptr->hashptr->key() == key) {
#ifdef CHECK_FIND
	std::cerr << "true" << std::endl;
#endif
	return;
      }
      _current_entryptr = _current_entryptr->next;
    }
  }
  _current_index = _container->_size;
  _current_entryptr = 0;  
#ifdef CHECK_FIND
  std::cerr << "false" << std::endl;
#endif
}

// destructor:
template<class HashData>
inline __pht_const_iterator<HashData>::~__pht_const_iterator() {}

// assignment:
template<class HashData>
inline __pht_const_iterator<HashData> __pht_const_iterator<HashData>::operator=(const __pht_const_iterator& iter) {
  if (this == &iter) {
    return *this;
  }
  _container = iter._container;
  _current_index = iter._current_index;
  _current_entryptr = iter._current_entryptr;
  return *this;
}

// accessors:
template<class HashData>
inline const PlainHashTable<HashData>* __pht_const_iterator<HashData>::container() const {
  return _container; 
}
template<class HashData>
inline const size_type __pht_const_iterator<HashData>::current_index() const {
  return _current_index;
}
template<class HashData>
inline const typename PlainHashTable<HashData>::entry* __pht_const_iterator<HashData>::current_entryptr() const {
  return current_entryptr;
}

// functions:
template<class HashData>
inline bool __pht_const_iterator<HashData>::operator==(const __pht_const_iterator& iter) 
  const {
    return (_current_entryptr == iter._current_entryptr);
  }
template<class HashData>
inline bool __pht_const_iterator<HashData>::operator!=(const __pht_const_iterator& iter)
  const {
    return !((*this) == iter);
  }

template<class HashData>
inline const HashData& __pht_const_iterator<HashData>::operator*() const {
  return *_current_entryptr->hashptr;
}
template<class HashData>
inline const HashData* __pht_const_iterator<HashData>::operator->() const {
  return _current_entryptr->hashptr;
}

template<class HashData>
inline __pht_const_iterator<HashData>& __pht_const_iterator<HashData>::operator++() {
  if ((_current_entryptr = _current_entryptr->next) != 0) {
    return *this;
  }
  while (++_current_index < _container->_size) {
    if ((_current_entryptr = _container->_data[_current_index]) != 0) {
      break;
    }
  }
  if (_current_index >= _container->_size) {
    _current_entryptr = 0;
  }
  return *this;
}
template<class HashData>
inline __pht_const_iterator<HashData> __pht_const_iterator<HashData>::operator++(int) {
  __pht_const_iterator tmp(*this);
  ++(*this);
  return tmp;
}

// find iterator:
template<class HashData>
inline __pht_iterator<HashData> PlainHashTable<HashData>::find(const key_type& key) {
#ifdef CHECK_FIND
  std::cerr << "searching for " << key << " in " << *this << std::endl;
#endif
  return iterator(*this, key);
}

// find const_iterator:
template<class HashData>
inline __pht_const_iterator<HashData> PlainHashTable<HashData>::find(const key_type& key) const {
#ifdef CHECK_FIND
  std::cerr << "const searching for " << key << " in " << *this << std::endl;
#endif
  return const_iterator(*this, key);
}

// Hash Objects:

template <class HashData>
HashKeySize<typename HashData::key_type> PlainHashTable<HashData>::_hashkeysize;
template <class HashData>
HashKey<typename HashData::key_type>     PlainHashTable<HashData>::_hashkey;

#ifdef WATCH_MAXCHAINLEN
template<class HashData>
size_type PlainHashTable<HashData>::_maxchainlen = 0;
#endif

template <class HashData>
typename PlainHashTable<HashData>::entry* PlainHashTable<HashData>::_bufptr = 0;

template <class HashData>
inline std::istream& operator>>(std::istream& ist, PlainHashTable<HashData>& p) {
  return p.read(ist);
}
template <class HashData>
inline std::ostream& operator<<(std::ostream& ost, const PlainHashTable<HashData>& p) {
  return p.write(ost);
}

#endif
// eof PlainHashTable.hh
