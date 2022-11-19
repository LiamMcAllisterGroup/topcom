////////////////////////////////////////////////////////////////////////////////
// 
// PlainUnlinkedHashTable.hh
//    produced: 22/01/2020 jr
// last change: 22/01/2020 jr
//
////////////////////////////////////////////////////////////////////////////////
#ifndef PLAINUNLINKEDHASHTABLE_HH
#define PLAINUNLINKEDHASHTABLE_HH

#include <stdlib.h>
#include <iostream>
#include <ctype.h>
#include <string.h>

#include <memory>

#include "Global.hh"
#include "RefCount.hh"
#include "HashKey.hh"

namespace topcom {

#ifndef _RAND_SEED
#define _RAND_SEED 19345
#endif

  // the following prime list is taken from STL, restricted to doubling:
  const size_type __puht_no_of_primes = 66;

  const size_type __puht_prime[__puht_no_of_primes] = {
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
  class __puht_entry {
  public:
    HashData*           hashptr;
    __puht_entry*        next;
  private:
    __puht_entry() {}
    __puht_entry(const __puht_entry&) {}
  public:
    inline __puht_entry(HashData* new_hashptr, __puht_entry* new_next) :
      hashptr(new_hashptr), next(new_next) {}
    inline ~__puht_entry() {}
  };

  template<class HashData>
  class PlainUnlinkedHashTable;

  template<class HashData>
  class __puht_iterator;

  template<class HashData>
  class __puht_const_iterator;

  template <class HashData>
  inline std::istream& operator>>(std::istream& ist, PlainUnlinkedHashTable<HashData>& p);

  template <class HashData>
  inline std::ostream& operator<<(std::ostream& ost, const PlainUnlinkedHashTable<HashData>& p);

  template<class HashData>
  class PlainUnlinkedHashTable {
  public:
    typedef __puht_entry<HashData> entry;
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
    entry**    _data;		// data of the PlainUnlinkedHashTable
    size_type  _current_prime;	// current index of prime used as size
    size_type  _bucket_count;    	// actual PlainUnlinkedHashTable size
    size_type  _size;		// number of elements in the table
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
    void        _rehash(const size_type new_bucket_count);
  private:
    inline const size_type _hash(const key_type& key) const;
  public:
    // constructors:
    inline PlainUnlinkedHashTable();
    PlainUnlinkedHashTable(const size_type); // create PlainUnlinkedHashTable of particular size
    inline PlainUnlinkedHashTable(const PlainUnlinkedHashTable&);
    // destructor:
    inline ~PlainUnlinkedHashTable();
    // memory management:
    inline const size_type reserve(const size_type);
    // assignment:
    PlainUnlinkedHashTable& operator=(const PlainUnlinkedHashTable& ht);
    // comparison:
    bool operator==(const PlainUnlinkedHashTable&) const;
    inline bool operator!=(const PlainUnlinkedHashTable&) const;
    // keys for containers:
    inline const size_type keysize()              const;
    const        size_type key(const size_type n) const;
    // accessors:
    inline const bool          empty()     const {return (_size == 0);}
    inline const size_type     card()         const {return _size;}
    inline const size_type     bucket_count() const {return _bucket_count;}
    inline const size_type     size()         const {return _size;}
    inline const entry**       data()         const {return _data;}
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
    friend std::istream& operator>><>(std::istream& ist, PlainUnlinkedHashTable<HashData>& p);
    friend std::ostream& operator<<<>(std::ostream& ost, const PlainUnlinkedHashTable<HashData>& p);
    // iterator:
    friend class __puht_iterator<HashData>;
    friend class __puht_const_iterator<HashData>;
    typedef __puht_iterator<HashData>       iterator;
    typedef __puht_const_iterator<HashData> const_iterator;
    // iterator functions:
    inline const_iterator begin()               const  {return const_iterator(*this);}
    inline const_iterator end()                 const  {return const_iterator(*this,0);}
    inline const_iterator find(const key_type&) const;
    inline iterator       begin()                      {return iterator(*this);}
    inline iterator       end()                        {return iterator(*this,0);}
    inline iterator       find(const key_type&);
  };


  template <class HashData>
  class __puht_iterator {
    typedef typename PlainUnlinkedHashTable<HashData>::key_type key_type;
  private:
    PlainUnlinkedHashTable<HashData>*                     _container;
    size_type                                     _current_index;
    typename PlainUnlinkedHashTable<HashData>::entry*     _current_entryptr;
  private:
    __puht_iterator() {}
  public:
    // constructors:
    __puht_iterator(PlainUnlinkedHashTable<HashData>&);
    inline __puht_iterator(PlainUnlinkedHashTable<HashData>&, int);
    inline __puht_iterator(const __puht_iterator&);
    __puht_iterator(PlainUnlinkedHashTable<HashData>&, const size_type);
    __puht_iterator(PlainUnlinkedHashTable<HashData>&, const key_type&);
    // destructor:
    inline ~__puht_iterator();
    // assignment:
    inline __puht_iterator operator=(const __puht_iterator& iter);
    // accessors:
    inline const PlainUnlinkedHashTable<HashData>*                 container()        const;
    inline const size_type                                 current_index()    const;
    inline const typename PlainUnlinkedHashTable<HashData>::entry* current_entryptr() const;
    // functions:
    inline bool       operator==(const __puht_iterator&) const;
    inline bool       operator!=(const __puht_iterator&) const;
  
    inline HashData&        operator*() const;
    inline HashData*        operator->() const;
    inline __puht_iterator&  operator++();
    inline __puht_iterator   operator++(int);
  };

  template <class HashData>
  class __puht_const_iterator {
    typedef typename PlainUnlinkedHashTable<HashData>::key_type key_type;
  private:
    const PlainUnlinkedHashTable<HashData>*                     _container;
    size_type                                           _current_index;
    const typename PlainUnlinkedHashTable<HashData>::entry*     _current_entryptr;
  private:
    __puht_const_iterator() {}
  public:
    // constructors:
    __puht_const_iterator(const PlainUnlinkedHashTable<HashData>&);
    inline __puht_const_iterator(const PlainUnlinkedHashTable<HashData>&, int);
    inline __puht_const_iterator(const __puht_const_iterator&);
    inline __puht_const_iterator(const __puht_iterator<HashData>&);
    __puht_const_iterator(const PlainUnlinkedHashTable<HashData>&, const size_type);
    __puht_const_iterator(const PlainUnlinkedHashTable<HashData>&, const key_type&);
    // destructor:
    inline ~__puht_const_iterator();
    // assignment:
    inline __puht_const_iterator operator=(const __puht_const_iterator& iter);
    // accessors:
    inline const PlainUnlinkedHashTable<HashData>*                 container()        const;
    inline const size_type                                 current_index()    const;
    inline const typename PlainUnlinkedHashTable<HashData>::entry* current_entryptr() const;
    // functions:
    inline bool                      operator==(const __puht_const_iterator&) const;
    inline bool                      operator!=(const __puht_const_iterator&) const;
  
    inline const  HashData&            operator*() const;
    inline const  HashData*            operator->() const;
    inline __puht_const_iterator&       operator++();
    inline        __puht_const_iterator operator++(int);
  };

  template<class HashData>
  inline void PlainUnlinkedHashTable<HashData>::_init_data(entry** init_data, const size_type init_bucket_count) {
    memset(init_data, 0, init_bucket_count * sizeof(entry*));
  }

  template<class HashData>
  void PlainUnlinkedHashTable<HashData>::_init_randvec(size_type* init_randvec, const size_type init_maxkey) {
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
  void PlainUnlinkedHashTable<HashData>::_term_data(entry** term_data, const size_type term_bucket_count) {
    for (size_type i = 0; i < term_bucket_count; ++i) {
      const entry* eptr = term_data[i];
      while (eptr) {
	const entry* delptr = eptr;
	eptr = eptr->next;
	HashData_allocator.destroy(delptr->hashptr);
	HashData_allocator.deallocate(delptr->hashptr, 1);
	entry_allocator.deallocate((entry*)delptr, 1);
      }
    }
    entryptr_allocator.deallocate(term_data, term_bucket_count);
  }

  template<class HashData>
  inline void PlainUnlinkedHashTable<HashData>::_term_randvec(size_type* term_randvec, const size_type term_bucket_count) {
    size_type_allocator.deallocate(term_randvec, term_bucket_count);
  }

  template<class HashData>
  void PlainUnlinkedHashTable<HashData>::_copy_data(entry** to_data, 
						    const entry*const* from_data,
						    const size_type from_bucket_count) {
    if (to_data == from_data) {
      return;
    }
    for (size_type j = 0; j < from_bucket_count; ++j) {
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
  inline void PlainUnlinkedHashTable<HashData>::_copy_randvec(size_type* to_randvec,
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
  void PlainUnlinkedHashTable<HashData>::_extend_randvec(const size_type new_maxkey) {
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
  inline void PlainUnlinkedHashTable<HashData>::_expand() {
    // if (_current_prime < __puht_no_of_primes - 1) {
    //   _rehash(__puht_prime[++_current_prime]);
    // }
    if (__puht_prime[_current_prime] == __puht_prime[_current_prime+1]) {
      return;
    }
    else {
      _rehash(__puht_prime[++_current_prime]);
    }
  }

  template<class HashData>
  inline void PlainUnlinkedHashTable<HashData>::_contract() {
    if ( (_current_prime > 1) || ((_current_prime > 0) && (_size == 0)) ) {
      _rehash(__puht_prime[--_current_prime]);
    }
  }

  template<class HashData>
  void PlainUnlinkedHashTable<HashData>::_rehash(const size_type new_bucket_count) {
#ifdef REHASH_DEBUG
    std::cout << "PlainUnlinkedHashTable::_rehash(" << new_bucket_count << "); old_bucket_count = " << _bucket_count << std::endl;
#endif
    if (new_bucket_count == _bucket_count) {
      return;
    }
#ifdef WATCH_MAXCHAINLEN
    // reset maximal chain length:
    _maxchainlen = 0;
#endif
    if (_bucket_count == 0) {
      _bucket_count = new_bucket_count;
      _data = entryptr_allocator.allocate(_bucket_count);
      for (size_type i = 0; i < _bucket_count; ++i) {
	_data[i] = 0;
      }
      return;
    }
    size_type old_bucket_count = _bucket_count;
    _bucket_count = new_bucket_count;				// _hash needs correct _bucket_count for correct hashing
    if (_bucket_count == 0) {
      if (_data) {
	entryptr_allocator.deallocate(_data, old_bucket_count);
	_data = 0;
      }
      return;
    }
    entry** dp = entryptr_allocator.allocate(_bucket_count);
    for (size_type i = 0; i < _bucket_count; ++i) {
      dp[i] = 0;
    }
    for (size_type i = 0; i < old_bucket_count; ++i) {
      entry* eptr = _data[i];
      while (eptr) {
	size_type hash_index = _hash(eptr->hashptr->key()); // new index
	_data[i] = eptr->next;	// unlink first element
	eptr->next = dp[hash_index]; // set next to first element at new hash_index
	dp[hash_index] = eptr;	// link as first element at new hash_index
	eptr = _data[i];
      }
    }
    entryptr_allocator.deallocate(_data, old_bucket_count);
    _data = dp;
  }

  template<class HashData>
  inline const size_type PlainUnlinkedHashTable<HashData>::_hash(const key_type& key) const {
    size_type result(0UL);
    const size_type keysize(_hashkeysize(key)); // store because this is performance critical
    for (size_type i = 0; i < keysize; ++i) {
      result ^= _hashkey(key, i) ^ ((result << 5) | (result >> keysize));
    }
    return result % _bucket_count;
  }

  // constructors:
  template<class HashData>
  inline PlainUnlinkedHashTable<HashData>::PlainUnlinkedHashTable() : 
    _data(0), 
    _current_prime(_init_prime), 
    _bucket_count(__puht_prime[_init_prime]), 
    _size(0),
    _randvec(0), 
    _maxkey(_init_maxkey) {
    if (_bucket_count > 0) {
      _data = entryptr_allocator.allocate(_bucket_count);
      _init_data(_data, _bucket_count);
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
  PlainUnlinkedHashTable<HashData>::PlainUnlinkedHashTable(const size_type new_size) :
    _data(0), 
    _current_prime(_init_prime), 
    _size(0),
    _randvec(0), 
    _maxkey(_init_maxkey) {
    const size_type target_bucket_count = new_size * 100 / _maxloadfactor;
    while (__puht_prime[_current_prime] < target_bucket_count) {
      ++_current_prime;
    }
    _bucket_count = __puht_prime[_current_prime];
    if (_bucket_count > 0) {
      _data = entryptr_allocator.allocate(_bucket_count);
      _init_data(_data, _bucket_count);
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
  inline PlainUnlinkedHashTable<HashData>::PlainUnlinkedHashTable(const PlainUnlinkedHashTable<HashData>& ht) :
    _data(0), 
    _current_prime(ht._current_prime),
    _bucket_count(ht._bucket_count),
    _size (ht._size),
    _randvec(0),
    _maxkey(ht._maxkey) {
    if (_bucket_count > 0) {
      _data = entryptr_allocator.allocate(_bucket_count);
      _copy_data(_data, ht._data, _bucket_count);
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

  // memory management:
  template <class T>
  inline const size_type PlainUnlinkedHashTable<T>::reserve(const size_type new_size) {
    const size_type target_bucket_count = new_size * 100 / _maxloadfactor;
    while (_bucket_count < target_bucket_count) {
      _expand();
    }
    return _bucket_count;
  }

  // destructor:
  template<class HashData>
  inline PlainUnlinkedHashTable<HashData>::~PlainUnlinkedHashTable() {
    if (_randvec) {
      _term_randvec(_randvec, _maxkey);
    }
    if (_data) {
      _term_data(_data, _bucket_count);
    }
  }

  // assignment:
  template<class HashData>
  PlainUnlinkedHashTable<HashData>& PlainUnlinkedHashTable<HashData>::operator=(const PlainUnlinkedHashTable<HashData>& ht) {
    if (this == &ht) {
      return *this;
    }
    // destroy old table:
    if (_data) {
      _term_data(_data, _bucket_count);
    }
    if (_randvec) {
      _term_randvec(_randvec, _maxkey);
    }
    // copy table:
    _current_prime = ht._current_prime;
    _bucket_count = ht._bucket_count;
    _size = ht._size;
    _maxkey = ht._maxkey;
    if (_bucket_count > 0) {
      _data = entryptr_allocator.allocate(_bucket_count);
      _copy_data(_data, ht._data, _bucket_count);
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
  bool PlainUnlinkedHashTable<HashData>::operator==(const PlainUnlinkedHashTable<HashData>& ht) const {
    if (size() != ht.size()) {
#ifdef DEBUG
      std::cerr << size() << " (load) not equal to " << ht.size() << std::endl;
      std::cerr << "thus " << *this << " not equal to " << ht << std::endl;
#endif
      return false;
    }
    for (typename PlainUnlinkedHashTable<HashData>::const_iterator iter = this->begin(); 
	 iter != this->end(); 
	 ++iter) {
      const typename PlainUnlinkedHashTable<HashData>::const_iterator finder(ht.find(iter->key()));
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
  inline bool PlainUnlinkedHashTable<HashData>::operator!=(const PlainUnlinkedHashTable<HashData>& ht) const {
    return !((*this) == ht);
  }

  // keys for containers:
  template<class HashData>
  inline const size_type PlainUnlinkedHashTable<HashData>::keysize() const {
    return 1;
  }
  template<class HashData>
  const size_type PlainUnlinkedHashTable<HashData>::key(const size_type n) const {
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
  const HashData* PlainUnlinkedHashTable<HashData>::member(const key_type& key) const {
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
  HashData* PlainUnlinkedHashTable<HashData>::member(const key_type& key) {
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
  HashData* PlainUnlinkedHashTable<HashData>::insert(const HashData& hd) {
    if (_bucket_count == 0) {
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
#ifdef WATCH_HASHCOLLISION
      std::cerr << "hash collision between " << hd.key()
		<< " and " << eptr->hashptr->key() << std::endl;
      std::cerr << "hashindex " << hash_index << std::endl;
      std::cerr << "key of " << hd.key() 
		<< " = " << _hashkey(hd.key(), 0) << std::endl;
      std::cerr << "key of " << eptr->hashptr->key() 
		<< " = " << _hashkey(eptr->hashptr->key(), 0) << std::endl;
#endif
      eptr = eptr->next;
#ifdef WATCH_MAXCHAINLEN
      ++chain_len;
#endif
    }
#ifdef VERBOSE_CHAINLEN
    if (chain_len > 0) {
      std::cout << "PlainUnlinkedHashTable<HashData>::insert(const key_type&): chaining" 
		<< std::endl;
      std::cout << "at hash_index: " << hash_index << ", _bucket_count: " << _bucket_count 
		<< ", chain length: " << chain_len << std::endl;
    }

#endif
    if (_size * 100 / _bucket_count > _maxloadfactor) {
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
    ++_size;
#ifdef WATCH_MAXCHAINLEN
    if (chain_len > _maxchainlen) {
      _maxchainlen = chain_len;
    }
#endif
    return _data[hash_index]->hashptr;
  }

  template<class HashData>
  void PlainUnlinkedHashTable<HashData>::erase(const key_type& key) {
    if (_bucket_count == 0) {
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
      --_size;
      if ((100 * _size > __puht_prime[_init_prime] * _maxloadfactor) && (_size * 200 / _bucket_count < _maxloadfactor)) {
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
	--_size;
	if ((100 * _size > __puht_prime[_init_prime] * _maxloadfactor) 
	    && (_size * 200 / _bucket_count < _maxloadfactor)) {
	  _contract();
	}
	return;
      }
      eptr = eptr->next;
    }  
  }

  template<class HashData>
  void PlainUnlinkedHashTable<HashData>::erase_random() {
    entry* eptr;
    size_type random_index(random() % _bucket_count);
    eptr = _data[random_index];
    while (!eptr) {
      random_index = (random_index + 1) % _bucket_count;
      eptr = _data[random_index];
    }
    erase(eptr->hashptr->key());
  }

  template<class HashData>
  void PlainUnlinkedHashTable<HashData>::clear() {
#ifdef REHASH_DEBUG
    std::cout << "PlainUnlinkedHashTable<HashData>::clear()" << std::endl;
#endif
    if (_bucket_count == 0) {
      return;
    }
    // destroy old table:
    if (_data) {
      _term_data(_data, _bucket_count);
    }
    if (_randvec) {
      _term_randvec(_randvec, _maxkey);
    }
    // default construction:
    _current_prime = _init_prime;
    _bucket_count = __puht_prime[_init_prime];
    _size = 0;
    _maxkey = _init_maxkey;
    if (_bucket_count > 0) {
      _data = entryptr_allocator.allocate(_bucket_count);
      _init_data(_data, _bucket_count);
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
  std::istream& PlainUnlinkedHashTable<HashData>::read(std::istream& ist) {
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
	  std::cerr << "PlainUnlinkedHashTable<HashData>::read(std::istream&):"
		    << "cannot find hash data." << std::endl;
#endif
	  ist.clear(std::ios::failbit);
	  return ist;
	}
      }
    }
    else {
#ifdef READ_DEBUG
      std::cerr << "PlainUnlinkedHashTable<HashData>::read(std::istream&):"
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
  std::ostream& PlainUnlinkedHashTable<HashData>::write(std::ostream& ost) const {
    size_type count = 0;

    ost << "[";
    for (typename PlainUnlinkedHashTable<HashData>::const_iterator iter = this->begin(); iter != this->end(); ++iter) {
      ost << *iter;
      count++;
      if (count < _size)
	ost << ",";
    }
    ost << "]";
    return ost;
  }

  // inlines for PlainUnlinkedHashTable<HashData>::iterator:

  // constructors:
  template<class HashData>
  __puht_iterator<HashData>::__puht_iterator(PlainUnlinkedHashTable<HashData>& ht) : 
    _current_index(0) {
    _container = &ht;
    while ((_current_index < _container->_bucket_count) && 
	   !_container->_data[_current_index]) {
      ++_current_index;
    }
    if (_current_index == _container->_bucket_count) {
      _current_entryptr = 0;
    }
    else {
      _current_entryptr = _container->_data[_current_index];
    }
  }
  template<class HashData>
  inline  __puht_iterator<HashData>::__puht_iterator(PlainUnlinkedHashTable<HashData>& ht, int) {
    _container = &ht;
    _current_index = _container->_bucket_count;
    _current_entryptr = 0;
  }
  template<class HashData>
  inline __puht_iterator<HashData>::__puht_iterator(const __puht_iterator& iter) {
    _container = iter._container;
    _current_index = iter._current_index;
    _current_entryptr = iter._current_entryptr;
  }
  template<class HashData>
  __puht_iterator<HashData>::__puht_iterator(PlainUnlinkedHashTable<HashData>& ht, const size_type index) {
    _container = &ht;
    if (index < _container->_bucket_count) {
      _current_index = index;
      _current_entryptr = _container->_data[_current_index];
    }
    else {
      _current_index = _container->_bucket_count;
      _current_entryptr = 0;
    }
  }
  template<class HashData>
  __puht_iterator<HashData>::__puht_iterator(PlainUnlinkedHashTable<HashData>& ht, const key_type& key) {
    _container = &ht;
#ifdef CHECK_FIND
    std::cerr << "iterator at " << key << " in " << *_container << std::endl;
#endif
    if ( (_container->_bucket_count != 0)
	 && (_container->_maxkey >= PlainUnlinkedHashTable<HashData>::_hashkeysize(key))
	 && (!_container->empty()) ) {
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
    _current_index = _container->_bucket_count;
    _current_entryptr = 0;  
#ifdef CHECK_FIND
    std::cerr << "false" << std::endl;
#endif
  }

  // destructor:
  template<class HashData>
  inline __puht_iterator<HashData>::~__puht_iterator() {}

  // assignment:
  template<class HashData>
  inline  __puht_iterator<HashData> __puht_iterator<HashData>::operator=(const __puht_iterator& iter) {
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
  inline const PlainUnlinkedHashTable<HashData>* __puht_iterator<HashData>::container() const {
    return _container; 
  }
  template<class HashData>
  inline const size_type __puht_iterator<HashData>::current_index() const {
    return _current_index;
  }
  template<class HashData>
  inline const typename PlainUnlinkedHashTable<HashData>::entry* __puht_iterator<HashData>::current_entryptr() const {
    return _current_entryptr;
  }

  // functions:
  template<class HashData>
  inline bool __puht_iterator<HashData>::operator==(const __puht_iterator& iter)
    const {
    return (_current_entryptr == iter._current_entryptr);
  }
  template<class HashData>
  inline bool __puht_iterator<HashData>::operator!=(const __puht_iterator& iter)
    const {
    return !((*this) == iter);
  }

  template<class HashData>
  inline HashData& __puht_iterator<HashData>::operator*() const{
    return *_current_entryptr->hashptr;
  }
  template<class HashData>
  inline HashData* __puht_iterator<HashData>::operator->() const {
    return _current_entryptr->hashptr;
  }

  template<class HashData>
  inline __puht_iterator<HashData>& __puht_iterator<HashData>::operator++() {
    if ((_current_entryptr = _current_entryptr->next) != 0) {
      return *this;
    }
    while (++_current_index < _container->_bucket_count) {
      if ((_current_entryptr = _container->_data[_current_index]) != 0) {
	break;
      }
    }
    if (_current_index >= _container->_bucket_count) {
      _current_entryptr = 0;
    }
    return *this;
  }
  template<class HashData>
  inline __puht_iterator<HashData> __puht_iterator<HashData>::operator++(int) {
    __puht_iterator tmp(*this);
    ++(*this);
    return tmp;
  }

  // inlines for PlainUnlinkedHashTable<HashData>::const_iterator:

  // constructors:
  template<class HashData>
  __puht_const_iterator<HashData>::__puht_const_iterator(const PlainUnlinkedHashTable<HashData>& ht) : 
    _current_index(0) {
    _container = &ht;
    while ((_current_index < _container->_bucket_count) && 
	   !_container->_data[_current_index]) {
      ++_current_index;
    }
    if (_current_index == _container->_bucket_count) {
      _current_entryptr = 0;
    }
    else {
      _current_entryptr = _container->_data[_current_index];
    }
  }
  template<class HashData>
  inline __puht_const_iterator<HashData>::__puht_const_iterator(const PlainUnlinkedHashTable<HashData>& ht, int) {
    _container = &ht;
    _current_index = _container->_bucket_count;
    _current_entryptr = 0;
  }
  template<class HashData>
  inline __puht_const_iterator<HashData>::__puht_const_iterator(const __puht_const_iterator& iter) {
    _container = iter._container;
    _current_index = iter._current_index;
    _current_entryptr = iter._current_entryptr;
  }
  template<class HashData>
  inline __puht_const_iterator<HashData>::__puht_const_iterator(const __puht_iterator<HashData>& iter) {
    _container = iter.container();
    _current_index = iter.current_index();
    _current_entryptr = iter.current_entryptr();
  }
  template<class HashData>
  __puht_const_iterator<HashData>::__puht_const_iterator(const PlainUnlinkedHashTable<HashData>& ht, const size_type index) {
    _container = &ht;
    if (index < _container->_bucket_count) {
      _current_index = index;
      _current_entryptr = _container->_data[_current_index];
    }
    else {
      _current_index = _container->_bucket_count;
      _current_entryptr = 0;
    }
  }
  template<class HashData>
  __puht_const_iterator<HashData>::__puht_const_iterator(const PlainUnlinkedHashTable<HashData>& ht, const key_type& key) {
    _container = &ht;
#ifdef CHECK_FIND
    std::cerr << "const iterator at " << key << " in " << *_container << std::endl;
#endif
    if ( (_container->_bucket_count != 0)
	 && (_container->_maxkey >= PlainUnlinkedHashTable<HashData>::_hashkeysize(key))
	 && (!_container->empty()) ) {
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
    _current_index = _container->_bucket_count;
    _current_entryptr = 0;  
#ifdef CHECK_FIND
    std::cerr << "false" << std::endl;
#endif
  }

  // destructor:
  template<class HashData>
  inline __puht_const_iterator<HashData>::~__puht_const_iterator() {}

  // assignment:
  template<class HashData>
  inline __puht_const_iterator<HashData> __puht_const_iterator<HashData>::operator=(const __puht_const_iterator& iter) {
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
  inline const PlainUnlinkedHashTable<HashData>* __puht_const_iterator<HashData>::container() const {
    return _container; 
  }
  template<class HashData>
  inline const size_type __puht_const_iterator<HashData>::current_index() const {
    return _current_index;
  }
  template<class HashData>
  inline const typename PlainUnlinkedHashTable<HashData>::entry* __puht_const_iterator<HashData>::current_entryptr() const {
    return _current_entryptr;
  }

  // functions:
  template<class HashData>
  inline bool __puht_const_iterator<HashData>::operator==(const __puht_const_iterator& iter) 
    const {
    return (_current_entryptr == iter._current_entryptr);
  }
  template<class HashData>
  inline bool __puht_const_iterator<HashData>::operator!=(const __puht_const_iterator& iter)
    const {
    return !((*this) == iter);
  }

  template<class HashData>
  inline const HashData& __puht_const_iterator<HashData>::operator*() const {
    return *_current_entryptr->hashptr;
  }
  template<class HashData>
  inline const HashData* __puht_const_iterator<HashData>::operator->() const {
    return _current_entryptr->hashptr;
  }

  template<class HashData>
  inline __puht_const_iterator<HashData>& __puht_const_iterator<HashData>::operator++() {
    if ((_current_entryptr = _current_entryptr->next) != 0) {
      return *this;
    }
    while (++_current_index < _container->_bucket_count) {
      if ((_current_entryptr = _container->_data[_current_index]) != 0) {
	break;
      }
    }
    if (_current_index >= _container->_bucket_count) {
      _current_entryptr = 0;
    }
    return *this;
  }
  template<class HashData>
  inline __puht_const_iterator<HashData> __puht_const_iterator<HashData>::operator++(int) {
    __puht_const_iterator tmp(*this);
    ++(*this);
    return tmp;
  }

  // find iterator:
  template<class HashData>
  inline __puht_iterator<HashData> PlainUnlinkedHashTable<HashData>::find(const key_type& key) {
#ifdef CHECK_FIND
    std::cerr << "searching for " << key << " in " << *this << std::endl;
#endif
    return iterator(*this, key);
  }

  // find const_iterator:
  template<class HashData>
  inline __puht_const_iterator<HashData> PlainUnlinkedHashTable<HashData>::find(const key_type& key) const {
#ifdef CHECK_FIND
    std::cerr << "const searching for " << key << " in " << *this << std::endl;
#endif
    return const_iterator(*this, key);
  }

  // Hash Objects:

  template <class HashData>
  HashKeySize<typename HashData::key_type> PlainUnlinkedHashTable<HashData>::_hashkeysize;
  template <class HashData>
  HashKey<typename HashData::key_type>     PlainUnlinkedHashTable<HashData>::_hashkey;

#ifdef WATCH_MAXCHAINLEN
  template<class HashData>
  size_type PlainUnlinkedHashTable<HashData>::_maxchainlen = 0;
#endif

  template <class HashData>
  typename PlainUnlinkedHashTable<HashData>::entry* PlainUnlinkedHashTable<HashData>::_bufptr = 0;

  template <class HashData>
  inline std::istream& operator>>(std::istream& ist, PlainUnlinkedHashTable<HashData>& p) {
    return p.read(ist);
  }
  template <class HashData>
  inline std::ostream& operator<<(std::ostream& ost, const PlainUnlinkedHashTable<HashData>& p) {
    return p.write(ost);
  }

}; // namespace topcom

#endif
// eof PlainUnlinkedHashTable.hh
