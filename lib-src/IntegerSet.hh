///////////////////////////////////////////////////////////////////////////////
// 
// IntegerSet.hh
//
//    produced: 21/08/97 jr
// last change: 24/01/99 jr
// 
////////////////////////////////////////////////////////////////////////////////
#ifndef INTEGERSET_HH
#define INTEGERSET_HH

#include <stdlib.h>
#include <assert.h>

#include <memory>

#include "Global.hh"
#include "PlainArray.hh"
#include "Array.hh"
#include "SparseIntegerSet.hh"


// structure to aid in counting bits
// stolen from the gcc-2.95 STL

#ifndef BIT_LINEAR_SEARCH
template<bool __dummy>
struct _Bit_count {
  static unsigned char _S_bit_count[256];
};

// Mapping from 8 bit unsigned integers to the index of the first one
// bit:
// stolen from the gcc-2.95 STL

template<bool __dummy>
struct _First_one {
  static unsigned char _S_first_one[256];
};
#endif

typedef size_type  block;

// const block     bit_one      = 1UL;
const block     all_bits        = ~0UL;
const size_type byte_len        = 8UL;
const size_type bytes_per_block = sizeof(block);
const size_type block_len       = sizeof(block) * byte_len;
const block     bit_one         = 1UL;

// general functions:

inline size_type calc_no_of_blocks(const size_type elem) {
  return elem / block_len  + 1;
}

class CompressedIntegerSet;
class SparseIntegerSet;
class __is_const_iterator;

class IntegerSet {
private:
  std::allocator<block> block_allocator;
public:
  friend class CompressedIntegerSet;
protected:
  size_type      _no_of_blocks;	// length of the bit representation
  size_type      _size;		// actual size of allocated memory
  size_type      _invariant;    // equal sets have equal invariants
  block*         _bitrep;	// bit representation of the set
#ifdef BITCONSTANTS
  static block   _bit[32];	// bits as constant masks
  static block   _not_bit[32];	// complement bits as constant masks
#endif
private:
#ifdef WATCH_MAXBLOCKNO
  static size_type  _maxblockno; // maximal blocklength
private:
  void set_maxblockno() {
    if (_no_of_blocks > _maxblockno)
      _maxblockno = _no_of_blocks;
	
  }
public:
  static size_type maxblockno() { return _maxblockno; }
#endif
private:
  void _expand(const size_type);
  void _contract(const size_type);
  IntegerSet(const size_type len, block* init);
public:
  // constructors:
  IntegerSet();					// constructor for empty set
  IntegerSet(const IntegerSet&); // copy constructor
  IntegerSet(const SparseIntegerSet&); // constructor for SparseIntegerSet
  IntegerSet(const size_type, const size_type);	// constructor set containing the given range
  IntegerSet(const size_type elem);	// constructor for one-element set
  IntegerSet(const size_type len, const size_type* init); // constructor with given integer array
  IntegerSet(const Array<size_type>&); // constructor with given Array of integers
  IntegerSet(const PlainArray<size_type>&); // constructor with given PlainArray of integers
  // destructor:
  ~IntegerSet();				// destructor
  // keys for containers:
  const size_type keysize()        const;
  const block     key(size_type n) const;

  // accessors:
  const size_type no_of_blocks()   const;
  const size_type size()           const;
  const size_type invariant()      const;

  // functions:
  const size_type card() const;	// cardinality

  // boolean functions:
  const bool contains(const size_type)   const; // membership
  const bool superset(const IntegerSet&) const; // superset relation
  const bool subset(const IntegerSet&)   const; // subset relation
  const bool disjoint(const IntegerSet&) const; // disjoint relation

  const bool is_empty()                  const; // test for empty set

  // iterators:
  friend class __is_const_iterator;
  typedef __is_const_iterator const_iterator;
  typedef const_iterator      iterator; // no non-const iterators

  // iterator functions:
  inline const_iterator begin()                    const; // iterator on first element
  inline const_iterator end()                      const; // iterator past last element
  inline const_iterator find(const size_type elem) const; // sets iterator to point to elem

  // boolean operators:
  const bool operator==(const IntegerSet&) const; // equality
  const bool operator!=(const IntegerSet&) const; // non-equality
  const bool operator<(const IntegerSet&) const; // order on intsets
  const bool operator>(const IntegerSet&) const; // reversed order

  const size_type operator[](const size_type) const; // pseudo-random-access
  
  IntegerSet& clear();			// erase all
  IntegerSet& fill(const size_type, const size_type); // fill in elements from start to stop
  IntegerSet& operator=(const IntegerSet&); // assignment
  
  // adding and deleting an element:
  IntegerSet& operator+=(const size_type); // add integer
  IntegerSet& operator-=(const size_type); // delete integer
  IntegerSet& operator^=(const size_type); // add if not contained/delete if contained

  // union, difference, and intersection with sets:
  IntegerSet& operator+=(const IntegerSet&); // union
  IntegerSet& operator-=(const IntegerSet&); // difference
  IntegerSet& operator*=(const IntegerSet&); // intersection
  IntegerSet& operator^=(const IntegerSet&); // symmetric difference
  
  // the same but a new set is returned:
  IntegerSet operator+(const size_type)   const; // add integer
  IntegerSet operator-(const size_type)   const; // delete integer
  IntegerSet operator+(const IntegerSet&) const; // union
  IntegerSet operator-(const IntegerSet&) const; // difference
  IntegerSet operator*(const IntegerSet&) const; // intersection
  IntegerSet operator^(const IntegerSet&) const; // symmetric difference

  // returns the cardinalities
  // 0, 1, or 2 (2 or more) of
  // the intersection of several IntegerSet's:
  const size_type intersection_card(const IntegerSet**, 
				    const size_type, 
				    size_type&) const;
  const size_type intersection_nonempty(const IntegerSet**, 
					const size_type, 
					size_type&) const;
  // iostream:
  std::istream& read(std::istream&);
  std::ostream& write(std::ostream&) const;
  friend std::istream& operator>>(std::istream& ist, IntegerSet& is) {
    return is.read(ist);
  }
  friend std::ostream& operator<<(std::ostream& ost, const IntegerSet& is) {
    return is.write(ost);
  }
};

class __is_const_iterator {
  friend class IntegerSet;
  friend class CompressedIntegerSet;
  friend class __cis_const_iterator;
private:
  const IntegerSet*  _container;
  size_type          _current_block;
  size_type          _current_bit;
public:
  inline __is_const_iterator();
  inline __is_const_iterator(const __is_const_iterator& iter);
  __is_const_iterator(const IntegerSet& s);
  inline __is_const_iterator(const IntegerSet& s, int);
private:
  inline __is_const_iterator(const IntegerSet& s, 
			     const size_type current_block,
			     const size_type current_bit);
public:
  inline __is_const_iterator& operator=(const __is_const_iterator& iter);
  inline const bool           valid() const;
  inline const bool           operator==(const __is_const_iterator&) const;
  inline const bool           operator!=(const __is_const_iterator&) const;
  inline const size_type      operator*() const;
  __is_const_iterator& operator++();
  inline __is_const_iterator  operator++(int);
};

// IntegerSet inlines:

inline const bool IntegerSet::operator!=(const IntegerSet& s) const {
  return (!((*this) == s));
}

inline const size_type IntegerSet::keysize() const {
//   return _no_of_blocks;
  return 1;
}

inline const block IntegerSet::key(size_type n) const {
#ifdef DEBUG
  if (n >= keysize())
    std::cerr << "key(size_type): index out of range." << std::endl;

#endif
//   return _bitrep[n];
  return _invariant;
}

inline const bool IntegerSet::is_empty() const {
#ifdef DEBUG
  for (size_type i = 0; i < _no_of_blocks; ++i)
    if (_bitrep[i] != 0)
      return 0;

  return 1;
#endif
  return (_no_of_blocks == 0);
}

inline IntegerSet::const_iterator IntegerSet::begin() const {
  return IntegerSet::const_iterator(*this);
}

inline IntegerSet::const_iterator IntegerSet::end() const {
  return IntegerSet::const_iterator(*this,0);
}

inline IntegerSet::const_iterator IntegerSet::find(const size_type elem) const {
  return IntegerSet::const_iterator(*this, elem / block_len, elem % block_len);
}

// IntegerSet::const_iterator inlines:
inline __is_const_iterator::__is_const_iterator() : 
  _container(0), 
  _current_block(0), 
  _current_bit(0) {
}

inline __is_const_iterator::__is_const_iterator(const __is_const_iterator& iter) :
  _container(iter._container),
  _current_block(iter._current_block),
  _current_bit(iter._current_bit) {
}


inline __is_const_iterator::__is_const_iterator(const IntegerSet& s, int) :
  _container(&s), 
  _current_block(_container->_no_of_blocks), 
  _current_bit(block_len) {}

inline __is_const_iterator::__is_const_iterator(const IntegerSet& s, 
						const size_type current_block,
						const size_type current_bit) :
  _container(&s), 
  _current_block(current_block), 
  _current_bit(current_bit) {
  assert(_container->contains(current_block * block_len + current_bit));
}

inline __is_const_iterator& __is_const_iterator::operator=(const __is_const_iterator& iter) {
  if (this == &iter) {
    return *this;
  }
  _container = iter._container;
  _current_block = iter._current_block;
  _current_bit = iter._current_bit;
  return *this;
}

inline const bool __is_const_iterator::valid() const {
  return (_current_bit < block_len);
}

inline const bool __is_const_iterator::operator==(const __is_const_iterator& iter) const {
  return ((_container == iter._container)
	  && (_current_block == iter._current_block)
	  && (_current_bit == iter._current_bit));
}

inline const bool __is_const_iterator::operator!=(const __is_const_iterator& iter) const {
  return !((*this) == iter);
}

inline const size_type __is_const_iterator::operator*() const {
#ifdef INDEX_CHECK
  if (!valid()) {
    std::cerr << "__is_const_iterator::operator*(): invalid __is_const_iterator."
	 << std::endl;
    exit(1);
  }
#endif
  return (_current_block * block_len + _current_bit);
}

inline __is_const_iterator __is_const_iterator::operator++(int) {
  __is_const_iterator tmp = *this;
  ++(*this);
  return tmp;
}

#if defined (STL_CONTAINERS) || defined (STL_SYMMETRIES) || defined (STL_CHIROTOPE) || defined (NEED_STL_HASH)

#include <tr1/unordered_map>
#include <tr1/unordered_set>

namespace std {
  namespace tr1 {
    template<>				
    struct hash<IntegerSet> {
      std::size_t operator()(const IntegerSet& is) const {
 	long res(0);
 	for (int i = 0; i < is.keysize(); ++i) {
 	  res ^= is.key(i);
 	}
 	return res;
      }
    };      
  };
};                                            
#endif

#endif
// eof IntegerSet.hh
