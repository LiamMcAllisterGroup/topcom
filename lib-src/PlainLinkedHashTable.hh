////////////////////////////////////////////////////////////////////////////////
// 
// PlainLinkedHashTable.hh
//    produced: 22/01/2020 jr
// last change: 22/01/2020 jr
//
////////////////////////////////////////////////////////////////////////////////
#ifndef PLAINLINKEDHASHTABLE_HH
#define PLAINLINKEDHASHTABLE_HH

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
  const size_type __plht_no_of_primes = 66;

  const size_type __plht_prime[__plht_no_of_primes] = {
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
  class __plht_entry {
  public:
    HashData*           hashptr;
    __plht_entry*        next;
    __plht_entry*        listnext;
    __plht_entry*        listprev;
  private:
    __plht_entry() {}
    __plht_entry(const __plht_entry&) {}
  public:
    inline __plht_entry(HashData* new_hashptr,
			__plht_entry* new_next,
			__plht_entry* new_listnext,
			__plht_entry* new_listprev) :
      hashptr(new_hashptr),
      next(new_next),
      listnext(new_listnext),
      listprev(new_listprev) {}
    inline ~__plht_entry() {}
  };

  template<class HashData>
  class PlainLinkedHashTable;

  template<class HashData>
  class __plht_iterator;

  template<class HashData>
  class __plht_const_iterator;

  template <class HashData>
  inline std::istream& operator>>(std::istream& ist, PlainLinkedHashTable<HashData>& p);

  template <class HashData>
  inline std::ostream& operator<<(std::ostream& ost, const PlainLinkedHashTable<HashData>& p);

  template<class HashData>
  class PlainLinkedHashTable {
  public:
    typedef __plht_entry<HashData> entry;
  public:
    std::allocator<HashData>                               HashData_allocator;
    std::allocator<entry*>                                 entryptr_allocator;
    std::allocator<entry>                                  entry_allocator;
    std::allocator<size_type>                              size_type_allocator;
    typedef typename HashData::const_keyptr_type           const_keyptr_type;
    typedef typename HashData::keyptr_type                 keyptr_type;
    typedef typename HashData::key_type                    key_type;
  private:
    HashKeySize<key_type> _hashkeysize;
    HashKey<key_type>     _hashkey;
  private:
    enum {_init_prime = 0UL, _init_maxkey = 0UL, _maxloadfactor = 100UL,
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
    // static entry* _bufptr;
  private:
    entry**    _data;		// data of the PlainLinkedHashTable
    entry*     _firstentry;       // pointer to the first entry
    entry*     _lastentry;        // pointer to the last entry
    size_type  _current_prime;	// current index of prime used as size
    size_type  _bucket_count;	// actual PlainLinkedHashTable size
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
    inline PlainLinkedHashTable();
    PlainLinkedHashTable(const size_type); // create PlainLinkedHashTable of particular size
    inline PlainLinkedHashTable(const PlainLinkedHashTable&);
    // destructor:
    inline ~PlainLinkedHashTable();
    // memory management:
    inline const size_type reserve(const size_type);
    // assignment:
    PlainLinkedHashTable& operator=(const PlainLinkedHashTable& ht);
    // comparison:
    bool operator==(const PlainLinkedHashTable&) const;
    inline bool operator!=(const PlainLinkedHashTable&) const;
    // keys for containers:
    inline const size_type keysize()              const;
    const        size_type key(const size_type n) const;
    // accessors:
    inline const bool          empty()        const {return (_size == 0);}
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
    friend std::istream& operator>><>(std::istream& ist, PlainLinkedHashTable<HashData>& p);
    friend std::ostream& operator<<<>(std::ostream& ost, const PlainLinkedHashTable<HashData>& p);
    // iterator:
    friend class __plht_iterator<HashData>;
    friend class __plht_const_iterator<HashData>;
    typedef __plht_iterator<HashData>       iterator;
    typedef __plht_const_iterator<HashData> const_iterator;
    // iterator functions:
    inline const_iterator begin()               const  {return const_iterator(*this);}
    inline const_iterator end()                 const  {return const_iterator(*this,0);}
    inline const_iterator find(const key_type&) const;
    inline iterator       begin()                      {return iterator(*this);}
    inline iterator       end()                        {return iterator(*this,0);}
    inline iterator       find(const key_type&);
  };


  template <class HashData>
  class __plht_iterator {
    typedef typename PlainLinkedHashTable<HashData>::key_type key_type;
  private:
    PlainLinkedHashTable<HashData>*                     _container;
    typename PlainLinkedHashTable<HashData>::entry*     _current_entryptr;
  private:
    __plht_iterator() {}
  public:
    // constructors:
    __plht_iterator(PlainLinkedHashTable<HashData>&);
    inline __plht_iterator(PlainLinkedHashTable<HashData>&, int);
    inline __plht_iterator(const __plht_iterator&);
    __plht_iterator(PlainLinkedHashTable<HashData>&, const size_type);
    __plht_iterator(PlainLinkedHashTable<HashData>&, const key_type&);
    // destructor:
    inline ~__plht_iterator();
    // assignment:
    inline __plht_iterator operator=(const __plht_iterator& iter);
    // accessors:
    inline const PlainLinkedHashTable<HashData>*                 container()        const;
    inline const typename PlainLinkedHashTable<HashData>::entry* current_entryptr() const;
    // functions:
    inline bool       operator==(const __plht_iterator&) const;
    inline bool       operator!=(const __plht_iterator&) const;
  
    inline HashData&        operator*() const;
    inline HashData*        operator->() const;
    inline __plht_iterator&  operator++();
    inline __plht_iterator   operator++(int);
  };

  template <class HashData>
  class __plht_const_iterator {
    typedef typename PlainLinkedHashTable<HashData>::key_type key_type;
  private:
    const PlainLinkedHashTable<HashData>*                     _container;
    const typename PlainLinkedHashTable<HashData>::entry*     _current_entryptr;
  private:
    __plht_const_iterator() {}
  public:
    // constructors:
    __plht_const_iterator(const PlainLinkedHashTable<HashData>&);
    inline __plht_const_iterator(const PlainLinkedHashTable<HashData>&, int);
    inline __plht_const_iterator(const __plht_const_iterator&);
    inline __plht_const_iterator(const __plht_iterator<HashData>&);
    __plht_const_iterator(const PlainLinkedHashTable<HashData>&, const size_type);
    __plht_const_iterator(const PlainLinkedHashTable<HashData>&, const key_type&);
    // destructor:
    inline ~__plht_const_iterator();
    // assignment:
    inline __plht_const_iterator operator=(const __plht_const_iterator& iter);
    // accessors:
    inline const PlainLinkedHashTable<HashData>*                 container()        const;
    inline const typename PlainLinkedHashTable<HashData>::entry* current_entryptr() const;
    // functions:
    inline bool                      operator==(const __plht_const_iterator&) const;
    inline bool                      operator!=(const __plht_const_iterator&) const;
  
    inline const  HashData&            operator*() const;
    inline const  HashData*            operator->() const;
    inline __plht_const_iterator&      operator++();
    inline __plht_const_iterator       operator++(int);
  };

  template<class HashData>
  inline void PlainLinkedHashTable<HashData>::_init_data(entry** init_data, const size_type init_bucket_count) {
    memset(init_data, 0, init_bucket_count * sizeof(entry*));
  }

  template<class HashData>
  void PlainLinkedHashTable<HashData>::_init_randvec(size_type* init_randvec, const size_type init_maxkey) {
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
  void PlainLinkedHashTable<HashData>::_term_data(entry** term_data, const size_type term_bucket_count) {
    for (size_type i = 0; i < term_bucket_count; ++i) {
      const entry* eptr = term_data[i];
      while (eptr) {
	const entry* delptr = eptr;
	eptr = eptr->next;
	std::allocator_traits<std::allocator<HashData> >::destroy(HashData_allocator, delptr->hashptr);
	HashData_allocator.deallocate(delptr->hashptr, 1);
	entry_allocator.deallocate((entry*)delptr, 1);
      }
    }
    entryptr_allocator.deallocate(term_data, term_bucket_count);
  }

  template<class HashData>
  inline void PlainLinkedHashTable<HashData>::_term_randvec(size_type* term_randvec, const size_type term_bucket_count) {
    size_type_allocator.deallocate(term_randvec, term_bucket_count);
  }

  template<class HashData>
  void PlainLinkedHashTable<HashData>::_copy_data(entry** to_data, 
						  const entry*const* from_data,
						  const size_type from_bucket_count) {
    if (to_data == from_data) {
      return;
    }
    // initialize the linked list:
    _firstentry = 0;
    _lastentry = 0;
    for (size_type j = 0; j < from_bucket_count; ++j) {
      to_data[j] = 0;
      const entry* eptr = from_data[j];
      while (eptr) {
	HashData* hashptr = HashData_allocator.allocate(1);
	std::allocator_traits<std::allocator<HashData> >::construct(HashData_allocator, hashptr, *eptr->hashptr);
	entry* _bufptr = to_data[j];
	to_data[j] = entry_allocator.allocate(1);
	to_data[j]->hashptr  = hashptr;
	to_data[j]->next     = _bufptr;
	// logically insert into end of list:
	if (_lastentry == 0) {
	  _firstentry = to_data[j];
	}
	else {
	  _lastentry->listnext = to_data[j];
	}
	to_data[j]->listprev = _lastentry;
	to_data[j]->listnext = 0;
	_lastentry = to_data[j];
	eptr = eptr->next;
      }
    }
  }

  template<class HashData>
  inline void PlainLinkedHashTable<HashData>::_copy_randvec(size_type* to_randvec,
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
  void PlainLinkedHashTable<HashData>::_extend_randvec(const size_type new_maxkey) {
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
  inline void PlainLinkedHashTable<HashData>::_expand() {
    // if (_current_prime < __plht_no_of_primes - 1) {
    //   _rehash(__plht_prime[++_current_prime]);
    // }
    if (__plht_prime[_current_prime] == __plht_prime[_current_prime+1]) {
      return;
    }
    else {
      _rehash(__plht_prime[++_current_prime]);
    }
  }

  template<class HashData>
  inline void PlainLinkedHashTable<HashData>::_contract() {
    if ( (_current_prime > 1) || ((_current_prime > 0) && (_size == 0)) ) {
      _rehash(__plht_prime[--_current_prime]);
    }
  }

  template<class HashData>
  void PlainLinkedHashTable<HashData>::_rehash(const size_type new_bucket_count) {
#ifdef REHASH_DEBUG
    std::cout << "PlainLinkedHashTable::_rehash(" << new_bucket_count << "); old_bucket_count = " << _bucket_count << std::endl;
#endif
    if (new_bucket_count == _bucket_count) {
      return;
    }
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
    // initialize the linked list:
    _firstentry = 0;
    _lastentry = 0;
    for (size_type i = 0; i < old_bucket_count; ++i) {
      entry* eptr = _data[i];
      while (eptr) {
	size_type hash_index = _hash(eptr->hashptr->key()); // new index
	_data[i] = eptr->next;	// unlink first element
	eptr->next = dp[hash_index]; // set next to first element at new hash_index
	dp[hash_index] = eptr;	// link as first element at new hash_index
	// logically insert into end of list:
	if (_lastentry == 0) {
	  _firstentry = eptr;
	}
	else {
	  _lastentry->listnext = eptr;
	}
	eptr->listprev = _lastentry;
	eptr->listnext = 0;
	_lastentry = eptr;
	// repeat with new first entry at hash index i:
	eptr = _data[i];
      }
    }
    entryptr_allocator.deallocate(_data, old_bucket_count);
    _data = dp;
  }

  template<class HashData>
  inline const size_type PlainLinkedHashTable<HashData>::_hash(const key_type& key) const {
    size_type result(0UL);
    const size_type keysize(_hashkeysize(key)); // store because this is performance critical
    for (size_type i = 0; i < keysize; ++i) {
      result ^= _hashkey(key, i) ^ ((result << 5) | (result >> keysize));
      // result ^= (result << 5) | (result >> keysize);
      // result += _hashkey(key, i);
    }
    return result % _bucket_count;
  }

  // constructors:
  template<class HashData>
  inline PlainLinkedHashTable<HashData>::PlainLinkedHashTable() : 
    _data(0),
    _firstentry(0),
    _lastentry(0),
    _current_prime(_init_prime), 
    _bucket_count(__plht_prime[_init_prime]), 
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
  PlainLinkedHashTable<HashData>::PlainLinkedHashTable(const size_type new_size) :
    _data(0), 
    _firstentry(0),
    _lastentry(0),
    _current_prime(_init_prime), 
    _size(0),
    _randvec(0), 
    _maxkey(_init_maxkey) {
    const size_type target_bucket_count = new_size * 100 / _maxloadfactor;
    while (__plht_prime[_current_prime] < target_bucket_count) {
      ++_current_prime;
    }
    _bucket_count = __plht_prime[_current_prime];
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
  inline PlainLinkedHashTable<HashData>::PlainLinkedHashTable(const PlainLinkedHashTable<HashData>& ht) :
    _data(0), 
    _firstentry(0),
    _lastentry(0),
    _current_prime(ht._current_prime),
    _bucket_count(ht._bucket_count),
    _size (ht._size),
    _randvec(ht._randvec),
    _maxkey(ht._maxkey) {
    if (_bucket_count > 0) {
      _data = entryptr_allocator.allocate(_bucket_count);
      _copy_data(_data, ht._data, _bucket_count);
    }
    if (_maxkey > 0) {
      _randvec = size_type_allocator.allocate(_maxkey);
      _copy_randvec(_randvec, ht._randvec, _maxkey);
    }
#ifdef DEBUG
    std::cerr << "*this after copy constructor: " << std::endl;
    std::cerr << *this << std::endl;
#endif
#ifdef CHECK_RANDVEC
    std::cerr << "_randvec after copy constructor:" << std::endl;
    for (size_type i = 0; i < _maxkey; ++i) {
      std::cerr << _randvec[i] << std::endl;
    }
#endif
  }

  // destructor:
  template<class HashData>
  inline PlainLinkedHashTable<HashData>::~PlainLinkedHashTable() {
    if (_randvec) {
      _term_randvec(_randvec, _maxkey);
    }
    if (_data) {
      _term_data(_data, _bucket_count);
    }
  }

  // memory management:
  template <class T>
  inline const size_type PlainLinkedHashTable<T>::reserve(const size_type new_size) {
    const size_type target_bucket_count = new_size * 100 / _maxloadfactor;
    while (_bucket_count < target_bucket_count) {
      _expand();
    }
    return _bucket_count;
  }

  // assignment:
  template<class HashData>
  PlainLinkedHashTable<HashData>& PlainLinkedHashTable<HashData>::operator=(const PlainLinkedHashTable<HashData>& ht) {
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
  bool PlainLinkedHashTable<HashData>::operator==(const PlainLinkedHashTable<HashData>& ht) const {
    if (size() != ht.size()) {
#ifdef DEBUG
      std::cerr << size() << " (load) not equal to " << ht.size() << std::endl;
      std::cerr << "thus " << *this << " not equal to " << ht << std::endl;
#endif
      return false;
    }
    for (typename PlainLinkedHashTable<HashData>::const_iterator iter = this->begin(); 
	 iter != this->end(); 
	 ++iter) {
      const typename PlainLinkedHashTable<HashData>::const_iterator finder(ht.find(iter->key()));
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
  inline bool PlainLinkedHashTable<HashData>::operator!=(const PlainLinkedHashTable<HashData>& ht) const {
    return !((*this) == ht);
  }

  // keys for containers:
  template<class HashData>
  inline const size_type PlainLinkedHashTable<HashData>::keysize() const {
    return 1;
  }
  template<class HashData>
  const size_type PlainLinkedHashTable<HashData>::key(const size_type n) const {
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
  const HashData* PlainLinkedHashTable<HashData>::member(const key_type& key) const {
    if (_bucket_count == 0) {
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
  HashData* PlainLinkedHashTable<HashData>::member(const key_type& key) {
    if (_bucket_count == 0) {
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
  HashData* PlainLinkedHashTable<HashData>::insert(const HashData& hd) {
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
      std::cout << "PlainLinkedHashTable<HashData>::insert(const key_type&): chaining" 
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
    std::allocator_traits<std::allocator<HashData> >::construct(HashData_allocator, hashptr, hd);
    entry* _bufptr = _data[hash_index];
    _data[hash_index] = entry_allocator.allocate(1);
    _data[hash_index]->hashptr = hashptr;
    _data[hash_index]->next = _bufptr;
    // insert into end of list:
    if (_lastentry == 0) {
      _firstentry = _data[hash_index];
    }
    else {
      _lastentry->listnext = _data[hash_index];
    }
    _data[hash_index]->listprev = _lastentry;
    _lastentry = _data[hash_index];
    _data[hash_index]->listnext = 0;
    ++_size;
#ifdef WATCH_MAXCHAINLEN
    if (chain_len > _maxchainlen) {
      _maxchainlen = chain_len;
    }
#endif
    return _data[hash_index]->hashptr;
  }

  template<class HashData>
  void PlainLinkedHashTable<HashData>::erase(const key_type& key) {
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
      if (delptr->listprev == 0) {
	// was first element in list:
	_firstentry = delptr->listnext;
      }
      else {
	delptr->listprev->listnext = delptr->listnext;
      }
      if (delptr->listnext == 0) {
	// was last element in list:
	_lastentry = delptr->listprev;
      }
      else {
	delptr->listnext->listprev = delptr->listprev;
      }
      std::allocator_traits<std::allocator<HashData> >::destroy(HashData_allocator, delptr->hashptr);
      HashData_allocator.deallocate(delptr->hashptr, 1);
      entry_allocator.deallocate(delptr, 1);
      --_size;
      if ((100 * _size > __plht_prime[_init_prime] * _maxloadfactor) && (_size * 200 / _bucket_count < _maxloadfactor)) {
	_contract();
      }
      return;
    }
    // delete element in chain at hash_index:
    while (eptr->next) {
      entry* delptr = eptr->next;
      if (eptr->next->hashptr->key() == key) {
	eptr->next = eptr->next->next;
	if (delptr->listprev == 0) {
	  // was first element in list:
	  _firstentry = delptr->listnext;
	}
	else {
	  delptr->listprev->listnext = delptr->listnext;
	}
	if (delptr->listnext == 0) {
	  // was last element in list:
	  _lastentry = delptr->listprev;
	}
	else {
	  delptr->listnext->listprev = delptr->listprev;
	}
	std::allocator_traits<std::allocator<HashData> >::destroy(HashData_allocator, delptr->hashptr);
	HashData_allocator.deallocate(delptr->hashptr, 1);
	entry_allocator.deallocate(delptr, 1);
	--_size;
	if ((100 * _size > __plht_prime[_init_prime] * _maxloadfactor) 
	    && (_size * 200 / _bucket_count < _maxloadfactor)) {
	  _contract();
	}
	return;
      }
      eptr = eptr->next;
    }  
  }

  template<class HashData>
  void PlainLinkedHashTable<HashData>::erase_random() {
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
  void PlainLinkedHashTable<HashData>::clear() {
#ifdef REHASH_DEBUG
    std::cout << "PlainLinkedHashTable<HashData>::clear()" << std::endl;
#endif
    if (_bucket_count == 0) {
      return;
    }
    // destroy old table:
    if (_data) {
      _term_data(_data, _bucket_count);
    }
    _firstentry = 0;
    _lastentry = 0;
    if (_randvec) {
      _term_randvec(_randvec, _maxkey);
    }
    // default construction:
    _current_prime = _init_prime;
    _bucket_count = __plht_prime[_init_prime];
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
  std::istream& PlainLinkedHashTable<HashData>::read(std::istream& ist) {
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
	  std::cerr << "PlainLinkedHashTable<HashData>::read(std::istream&):"
		    << "cannot find hash data." << std::endl;
#endif
	  ist.clear(std::ios::failbit);
	  return ist;
	}
      }
    }
    else {
#ifdef READ_DEBUG
      std::cerr << "PlainLinkedHashTable<HashData>::read(std::istream&):"
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
  std::ostream& PlainLinkedHashTable<HashData>::write(std::ostream& ost) const {
    size_type count = 0;

    ost << "[";
    for (typename PlainLinkedHashTable<HashData>::const_iterator iter = this->begin(); iter != this->end(); ++iter) {
      ost << *iter;
      count++;
      if (count < _size)
	ost << ",";
    }
    ost << "]";
    return ost;
  }

  // inlines for PlainLinkedHashTable<HashData>::iterator:

  // constructors:
  template<class HashData>
  __plht_iterator<HashData>::__plht_iterator(PlainLinkedHashTable<HashData>& ht) {
    _container = &ht;
    _current_entryptr = _container->_firstentry;
  }
  template<class HashData>
  inline  __plht_iterator<HashData>::__plht_iterator(PlainLinkedHashTable<HashData>& ht, int) {
    _container = &ht;
    _current_entryptr = 0;
  }
  template<class HashData>
  inline __plht_iterator<HashData>::__plht_iterator(const __plht_iterator& iter) {
    _container = iter._container;
    _current_entryptr = iter._current_entryptr;
  }
  template<class HashData>
  __plht_iterator<HashData>::__plht_iterator(PlainLinkedHashTable<HashData>& ht, const size_type index) {
    _container = &ht;
    if (index < _container->_bucket_count) {
      size_type current_index(index);
      while ((current_index < _container->_bucket_count) && (_container->_data[current_index] == 0)) {
	++current_index;
      }
      if (current_index == _container->_bucket_count) {
	_current_entryptr = 0;
      }
      else {
	_current_entryptr = _container->_data[current_index];
      }
    }
    else {
      _current_entryptr = 0;
    }
  }
  template<class HashData>
  __plht_iterator<HashData>::__plht_iterator(PlainLinkedHashTable<HashData>& ht, const key_type& key) {
    _container = &ht;
#ifdef CHECK_FIND
    std::cerr << "iterator at " << key << " in " << *_container << std::endl;
#endif
    if ( (_container->_bucket_count != 0)
	 && (_container->_maxkey >= _container->_hashkeysize(key))
	 && (!_container->empty()) ) {
      const size_type find_index = _container->_hash(key);
      _current_entryptr = _container->_data[find_index];
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
    _current_entryptr = 0;  
#ifdef CHECK_FIND
    std::cerr << "false" << std::endl;
#endif
  }

  // destructor:
  template<class HashData>
  inline __plht_iterator<HashData>::~__plht_iterator() {}

  // assignment:
  template<class HashData>
  inline  __plht_iterator<HashData> __plht_iterator<HashData>::operator=(const __plht_iterator& iter) {
    if (this == &iter) {
      return *this;
    }
    _container = iter._container;
    _current_entryptr = iter._current_entryptr;
    return *this;
  }

  // accessors:
  template<class HashData>
  inline const PlainLinkedHashTable<HashData>* __plht_iterator<HashData>::container() const {
    return _container; 
  }
  template<class HashData>
  inline const typename PlainLinkedHashTable<HashData>::entry* __plht_iterator<HashData>::current_entryptr() const {
    return _current_entryptr;
  }

  // functions:
  template<class HashData>
  inline bool __plht_iterator<HashData>::operator==(const __plht_iterator& iter)
    const {
    return (_current_entryptr == iter._current_entryptr);
  }
  template<class HashData>
  inline bool __plht_iterator<HashData>::operator!=(const __plht_iterator& iter)
    const {
    return !((*this) == iter);
  }

  template<class HashData>
  inline HashData& __plht_iterator<HashData>::operator*() const{
    return *_current_entryptr->hashptr;
  }
  template<class HashData>
  inline HashData* __plht_iterator<HashData>::operator->() const {
    return _current_entryptr->hashptr;
  }

  template<class HashData>
  inline __plht_iterator<HashData>& __plht_iterator<HashData>::operator++() {
    _current_entryptr = _current_entryptr->listnext;
    return *this;
  }
  template<class HashData>
  inline __plht_iterator<HashData> __plht_iterator<HashData>::operator++(int) {
    __plht_iterator tmp(*this);
    ++(*this);
    return tmp;
  }

  // inlines for PlainLinkedHashTable<HashData>::const_iterator:

  // constructors:
  template<class HashData>
  __plht_const_iterator<HashData>::__plht_const_iterator(const PlainLinkedHashTable<HashData>& ht) {
    _container = &ht;
    _current_entryptr = _container->_firstentry;
  }
  template<class HashData>
  inline __plht_const_iterator<HashData>::__plht_const_iterator(const PlainLinkedHashTable<HashData>& ht, int) {
    _container = &ht;
    _current_entryptr = 0;
  }
  template<class HashData>
  inline __plht_const_iterator<HashData>::__plht_const_iterator(const __plht_const_iterator& iter) {
    _container = iter._container;
    _current_entryptr = iter._current_entryptr;
  }
  template<class HashData>
  inline __plht_const_iterator<HashData>::__plht_const_iterator(const __plht_iterator<HashData>& iter) {
    _container = iter.container();
    _current_entryptr = iter.current_entryptr();
  }
  template<class HashData>
  __plht_const_iterator<HashData>::__plht_const_iterator(const PlainLinkedHashTable<HashData>& ht, const size_type index) {
    _container = &ht;
    if (index < _container->_bucket_count) {
      size_type current_index(index);
      while ((current_index < _container->_bucket_count) && (_container->_data[current_index] == 0)) {
	++current_index;
      }
      if (current_index == _container->_bucket_count) {
	_current_entryptr = 0;
      }
      else {
	_current_entryptr = _container->_data[current_index];
      }
    }
    else {
      _current_entryptr = 0;
    }
  }
  template<class HashData>
  __plht_const_iterator<HashData>::__plht_const_iterator(const PlainLinkedHashTable<HashData>& ht, const key_type& key) {
    _container = &ht;
#ifdef CHECK_FIND
    std::cerr << "const iterator at " << key << " in " << *_container << std::endl;
#endif
    if ( (_container->_bucket_count != 0)
	 && (_container->_maxkey >= _container->_hashkeysize(key))
	 && (!_container->empty()) ) {
      const size_type find_index = _container->_hash(key);
      _current_entryptr = _container->_data[find_index];
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
    _current_entryptr = 0;  
#ifdef CHECK_FIND
    std::cerr << "false" << std::endl;
#endif
  }

  // destructor:
  template<class HashData>
  inline __plht_const_iterator<HashData>::~__plht_const_iterator() {}

  // assignment:
  template<class HashData>
  inline __plht_const_iterator<HashData> __plht_const_iterator<HashData>::operator=(const __plht_const_iterator& iter) {
    if (this == &iter) {
      return *this;
    }
    _container = iter._container;
    _current_entryptr = iter._current_entryptr;
    return *this;
  }

  // accessors:
  template<class HashData>
  inline const PlainLinkedHashTable<HashData>* __plht_const_iterator<HashData>::container() const {
    return _container; 
  }
  template<class HashData>
  inline const typename PlainLinkedHashTable<HashData>::entry* __plht_const_iterator<HashData>::current_entryptr() const {
    return _current_entryptr;
  }

  // functions:
  template<class HashData>
  inline bool __plht_const_iterator<HashData>::operator==(const __plht_const_iterator& iter) const {
    return (_current_entryptr == iter._current_entryptr);
  }
  template<class HashData>
  inline bool __plht_const_iterator<HashData>::operator!=(const __plht_const_iterator& iter)
    const {
    return !((*this) == iter);
  }

  template<class HashData>
  inline const HashData& __plht_const_iterator<HashData>::operator*() const {
    return *_current_entryptr->hashptr;
  }
  template<class HashData>
  inline const HashData* __plht_const_iterator<HashData>::operator->() const {
    return _current_entryptr->hashptr;
  }

  template<class HashData>
  inline __plht_const_iterator<HashData>& __plht_const_iterator<HashData>::operator++() {
    _current_entryptr = _current_entryptr->listnext;
    return *this;
  }
  template<class HashData>
  inline __plht_const_iterator<HashData> __plht_const_iterator<HashData>::operator++(int) {
    __plht_const_iterator tmp(*this);
    ++(*this);
    return tmp;
  }

  // find iterator:
  template<class HashData>
  inline __plht_iterator<HashData> PlainLinkedHashTable<HashData>::find(const key_type& key) {
#ifdef CHECK_FIND
    std::cerr << "searching for " << key << " in " << *this << std::endl;
#endif
    return iterator(*this, key);
  }

  // find const_iterator:
  template<class HashData>
  inline __plht_const_iterator<HashData> PlainLinkedHashTable<HashData>::find(const key_type& key) const {
#ifdef CHECK_FIND
    std::cerr << "const searching for " << key << " in " << *this << std::endl;
#endif
    return const_iterator(*this, key);
  }

  // Hash Objects:

  // template <class HashData>
  // HashKeySize<typename HashData::key_type> PlainLinkedHashTable<HashData>::_hashkeysize;
  // template <class HashData>
  // HashKey<typename HashData::key_type>     PlainLinkedHashTable<HashData>::_hashkey;

#ifdef WATCH_MAXCHAINLEN
  template<class HashData>
  size_type PlainLinkedHashTable<HashData>::_maxchainlen = 0;
#endif

  // template <class HashData>
  // typename PlainLinkedHashTable<HashData>::entry* PlainLinkedHashTable<HashData>::_bufptr = 0;

  template <class HashData>
  inline std::istream& operator>>(std::istream& ist, PlainLinkedHashTable<HashData>& p) {
    return p.read(ist);
  }
  template <class HashData>
  inline std::ostream& operator<<(std::ostream& ost, const PlainLinkedHashTable<HashData>& p) {
    return p.write(ost);
  }

}; // namespace topcom

#endif
// eof PlainLinkedHashTable.hh
