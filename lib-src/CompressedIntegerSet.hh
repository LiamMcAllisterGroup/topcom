////////////////////////////////////////////////////////////////////////////////
// 
// CompressedIntegerSet.hh
//
//    produced: 21/08/97 jr
// last change: 24/01/99 jr
// 
////////////////////////////////////////////////////////////////////////////////
#ifndef COMPRESSEDINTEGERSET_HH
#define COMPRESSEDINTEGERSET_HH

#include <stdlib.h>

#include <memory>

#include "Global.hh"
#include "PlainArray.hh"
#include "Array.hh"
#include "IntegerSet.hh"

typedef Array<block> compressed_type;

class __cis_const_iterator; // same as for IntegerSet

class CompressedIntegerSet {
private:
  IntegerSet               _non_zero; // contains positions of non-zero blocks
  compressed_type          _compressed; // contains the non-zero blocks
public:

  // constructors:
  CompressedIntegerSet();
  CompressedIntegerSet(const CompressedIntegerSet&);
  CompressedIntegerSet(const IntegerSet&);

  // assignment:
  CompressedIntegerSet& operator=(const CompressedIntegerSet&);

  // conversion:
  operator IntegerSet() const;

  // interface wrapper of an IntegerSet in order to make compression transparent:
  CompressedIntegerSet(const SparseIntegerSet&); // constructor for SparseIntegerSet
  CompressedIntegerSet(const size_type, const size_type);	// constructor set containing the given range
  CompressedIntegerSet(const size_type elem);	// constructor for one-element set
  CompressedIntegerSet(const size_type len, const size_type* init); // constructor with given integer array
  CompressedIntegerSet(const Array<size_type>&); // constructor with given Array of integers
  CompressedIntegerSet(const PlainArray<size_type>&); // constructor with given PlainArray of integers
  // destructor:
  ~CompressedIntegerSet();				// destructor
  // keys for containers:
  const size_type keysize()        const;
  const size_type key(size_type n) const;

  // accessors:
  const IntegerSet&        non_zero()   const;
  const compressed_type&   compressed() const;

  // functions:
  const size_type card() const;	// cardinality

  // boolean functions:
  const bool contains(const size_type)   const; // membership
  const bool superset(const CompressedIntegerSet&) const; // superset relation
  const bool subset(const CompressedIntegerSet&)   const; // subset relation
  const bool disjoint(const CompressedIntegerSet&) const; // disjoint relation

  const bool is_empty()                  const; // test for empty set

  // iterators:
  friend class __cis_const_iterator; // same as for IntegerSet
  typedef __cis_const_iterator const_iterator;
  typedef const_iterator       iterator; // no non-const iterators
  // iterator functions:
  const_iterator begin() const;	// iterator on first element
  const_iterator end()   const;	// iterator past last element
  const_iterator find(const size_type elem) const; // iterator pointing to elem

  // boolean operators:
  const bool operator==(const CompressedIntegerSet&) const; // equality
  const bool operator!=(const CompressedIntegerSet&) const; // non-equality
  const bool operator<(const CompressedIntegerSet&) const; // order on intsets
  const bool operator>(const CompressedIntegerSet&) const; // reversed order

  const size_type operator[](const size_type) const; // pseudo-random-access
  
  CompressedIntegerSet& clear();			// erase all
  CompressedIntegerSet& fill(const size_type, const size_type); // fill in elements from start to stop
  
  // adding and deleting an element:
  CompressedIntegerSet& operator+=(const size_type); // add integer
  CompressedIntegerSet& operator-=(const size_type); // delete integer
  CompressedIntegerSet& operator^=(const size_type); // add if not contained/delete if contained

  // union, difference, and intersection with sets:
  CompressedIntegerSet& operator+=(const CompressedIntegerSet&); // union
  CompressedIntegerSet& operator-=(const CompressedIntegerSet&); // difference
  CompressedIntegerSet& operator*=(const CompressedIntegerSet&); // intersection
  CompressedIntegerSet& operator^=(const CompressedIntegerSet&); // symmetric difference
  
  // the same but a new set is returned:
  CompressedIntegerSet operator+(const size_type)   const; // add integer
  CompressedIntegerSet operator-(const size_type)   const; // delete integer
  CompressedIntegerSet operator+(const CompressedIntegerSet&) const; // union
  CompressedIntegerSet operator-(const CompressedIntegerSet&) const; // difference
  CompressedIntegerSet operator*(const CompressedIntegerSet&) const; // intersection
  CompressedIntegerSet operator^(const CompressedIntegerSet&) const; // symmetric difference

  // returns the cardinalities
  // 0, 1, or 2 (2 or more) of
  // the intersection of several CompressedIntegerSet's:
  const size_type intersection_card(const CompressedIntegerSet**, 
					   const size_type, 
					   size_type&) const;
  const size_type intersection_nonempty(const CompressedIntegerSet**, 
					       const size_type, 
					       size_type&) const;
  // iostream:
  std::istream& read(std::istream&);
  std::ostream& write(std::ostream&) const;
  friend std::istream& operator>>(std::istream& ist, CompressedIntegerSet& is) {
    return is.read(ist);
  }
  friend std::ostream& operator<<(std::ostream& ost, const CompressedIntegerSet& is) {
    return is.write(ost);
  }

private:

  // internal functions:
  void _compactify();
};

class __cis_const_iterator {
  friend class CompressedIntegerSet;
private:
  const CompressedIntegerSet*  _container;
  size_type                    _current_non_zero;
  size_type                    _current_block;
  size_type                    _current_bit;
public:
  __cis_const_iterator();
  __cis_const_iterator(const __cis_const_iterator& iter);
  __cis_const_iterator(const CompressedIntegerSet& s);
  __cis_const_iterator(const CompressedIntegerSet& s, int);
private:
  __cis_const_iterator(const CompressedIntegerSet& s,
		       const size_type current_non_zero,
		       const size_type current_block,
		       const size_type current_bit);
public:
  ~__cis_const_iterator();
  
  __cis_const_iterator& operator=(const __cis_const_iterator& iter);
  const bool           valid() const;
  const bool           operator==(const __cis_const_iterator&) const;
  const bool           operator!=(const __cis_const_iterator&) const;
  const size_type      operator*() const;
  __cis_const_iterator& operator++();
  __cis_const_iterator  operator++(int);
};


#endif
// eof CompressedIntegerSet.hh
