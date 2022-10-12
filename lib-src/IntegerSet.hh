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
#include "SparseIntegerSet.hh"

#ifndef STL_CONTAINERS
#include "PlainArray.hh"
#include "Array.hh"
#else
#include <vector>
#include <set>
#endif

namespace topcom {

  class Symmetry;

  constexpr block_type     no_bits         {0x0000000000000000};
  constexpr block_type     all_bits        {0xffffffffffffffff};
  constexpr block_type     bit_one         {0x0000000000000001};
  constexpr block_type     byte_one        {0x00000000000000ff};
  constexpr size_type      byte_len        {8};
  constexpr size_type      bytes_per_block {sizeof(block_type)};
  constexpr size_type      block_len       {sizeof(block_type) * byte_len};
  constexpr size_type      zero_size       {0};
  constexpr size_type      unit_size       {1};

  // general functions:

  inline size_type calc_no_of_blocks(const size_type elem) {
    return elem / block_len + 1;
  }

  class CompressedIntegerSet;
  class __cis_const_iterator;
  class SparseIntegerSet;
  class __is_const_iterator;

  class IntegerSet {
  private:
    std::allocator<block_type> block_allocator;
  public:
    friend class CompressedIntegerSet;
    friend class __cis_const_iterator;
  protected:
    size_type           _no_of_blocks; // length of the bit representation
    size_type           _memsize;      // actual size of allocated memory
    block_type          _invariant;    // equal sets have equal invariants
    block_type*         _bitrep;       // bit representation of the set
#ifdef BITCONSTANTS
    static block_type   _bit[64];      // bits as constant masks
    static block_type   _not_bit[64];  // complement bits as constant masks
#endif
#ifndef BIT_LINEAR_SEARCH
    static unsigned char _S_bit_count[256];
    static unsigned char _S_first_one[256];
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
  public:
    std::ostream& report_internals(std::ostream&) const;
  private:
    IntegerSet(const size_type len, block_type* init);
  public:
    // constructors:
    IntegerSet();					    // constructor for empty set
    IntegerSet(const IntegerSet&);                          // copy constructor
    IntegerSet(IntegerSet&&);                               // move constructor

    // copy constructor from permuted IntegerSet:
    IntegerSet(const IntegerSet&,
	       const Symmetry&,
	       const size_type = 0UL); // assume this bound on the maximum element of result
    IntegerSet(const SparseIntegerSet&);                    // constructor for SparseIntegerSet
    explicit IntegerSet(const size_type, const size_type);  // constructor set containing the given range
    explicit IntegerSet(const size_type elem);	            // constructor for one-element set
    IntegerSet(const size_type len, const size_type* init); // constructor with given integer array
#ifndef STL_CONTAINERS
    IntegerSet(const Array<size_type>&);                    // constructor with given Array of integers
    IntegerSet(const PlainArray<size_type>&);               // constructor with given PlainArray of integers
#else
    explicit IntegerSet(const std::set<size_type>&);        // constructor with given std::set of size_types
    explicit IntegerSet(const std::set<parameter_type>&);   // constructor with given std::set of parameter_types
    explicit IntegerSet(const std::vector<size_type>&);     // constructor with given std::vector of size_types
    explicit IntegerSet(const std::vector<parameter_type>&);// constructor with given std::vector of parameter_types
#endif
    // destructor:
    ~IntegerSet();   	     			            // destructor
    // keys for containers:
    const size_type keysize()        const;
    const size_type key(size_type n) const;

    // accessors:
    const size_type  no_of_blocks()   const;
    const size_type  memsize()        const;
    const block_type invariant()      const;

    // conversions:
    std::set<size_type>         convert_to_set_of_size_types()         const;
    std::set<parameter_type>    convert_to_set_of_parameter_types()    const;
    std::vector<size_type>      convert_to_vector_of_size_types()      const;
    std::vector<parameter_type> convert_to_vector_of_parameter_types() const;

    // functions:
    const size_type card()     const; // cardinality
    const size_type max_elem() const; // returns the maximal element (std::numeric_limits<size_type>::max() for empty set)
    const size_type min_elem() const; // returns the minimal element (std::numeric_limits<size_type>::max() for empty set)

    // boolean functions:
    const bool contains    (const size_type)   const; // membership
    const bool superset    (const IntegerSet&) const; // superset relation
    const bool subset      (const IntegerSet&) const; // subset relation
    const bool disjoint    (const IntegerSet&) const; // disjoint relation
    
    // true lexicographic/rev-colex order (may be slower than "<"):
    const bool lexsmaller  (const IntegerSet&) const;
    const bool colexgreater(const IntegerSet&) const;

    const bool empty() const; // test for empty set

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
    IntegerSet& fill(const size_type,
		     const size_type); // fill in elements from start to stop
    IntegerSet& operator=(const IntegerSet&); // copyassignment
    IntegerSet& operator=(IntegerSet&&); // move assignment
  
    // adding and deleting an element:
    IntegerSet& operator+=(const size_type); // add integer
    IntegerSet& operator-=(const size_type); // delete integer
    IntegerSet& operator^=(const size_type); // add if not contained/delete if contained

    // union, difference, and intersection with sets:
    IntegerSet& operator+=(const IntegerSet&); // union
    IntegerSet& operator-=(const IntegerSet&); // difference
    IntegerSet& operator*=(const IntegerSet&); // intersection
    IntegerSet& operator^=(const IntegerSet&); // symmetric difference

    // other modifiers:
    IntegerSet& remove_max(const size_type);
    IntegerSet& remove_min(const size_type);
    IntegerSet& permute(const Symmetry&);
  
    // the same but a new set is returned:
    IntegerSet operator+(const size_type)   const; // add integer
    IntegerSet operator-(const size_type)   const; // delete integer
    IntegerSet operator+(const IntegerSet&) const; // union
    IntegerSet operator-(const IntegerSet&) const; // difference
    IntegerSet operator*(const IntegerSet&) const; // intersection
    IntegerSet operator^(const IntegerSet&) const; // symmetric difference

    // other out-of-place functions:
    IntegerSet lexmin_subset(const size_type) const;
    IntegerSet lexmax_subset(const size_type) const;
    IntegerSet permute(const Symmetry&) const;

    // returns the cardinalities
    // 0, 1, or 2 (2 or more) of
    // the intersection of several IntegerSet's:
    const int intersection_card(const IntegerSet**, 
				const size_type, 
				size_type&) const;
    // specialization returning 0 (no element) or 1 (at least one element):
    const int intersection_nonempty(const IntegerSet**, 
				    const size_type, 
				    size_type&) const;
    // specialization for only one other IntegerSet:
    const bool intersection_nonempty(const IntegerSet&) const;

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
    return 1;
  }

  inline const size_type IntegerSet::key(size_type n) const {
#ifdef DEBUG
    if (n >= keysize())
      std::cerr << "key(size_type): index out of range." << std::endl;

#endif
    return static_cast<size_type>(_invariant);
  }

  inline const size_type IntegerSet::no_of_blocks() const {
    return _no_of_blocks;
  }

  inline const size_type IntegerSet::memsize() const {
    return _memsize;
  }

  inline const block_type IntegerSet::invariant() const {
    return _invariant;
  }

  inline std::set<size_type> IntegerSet::convert_to_set_of_size_types() const {
    std::set<size_type> result;
    for (const_iterator iter = begin();
	 iter != end();
	 ++iter) {
      result.insert(*iter);
    }
    return result;
  }
  
  inline std::set<parameter_type> IntegerSet::convert_to_set_of_parameter_types() const {
    std::set<parameter_type> result;
    for (const_iterator iter = begin();
	 iter != end();
	 ++iter) {
      result.insert(*iter);
    }
    return result;
  }

  inline std::vector<size_type> IntegerSet::convert_to_vector_of_size_types() const {
    std::vector<size_type> result;
    result.reserve(card());
    for (const_iterator iter = begin();
	 iter != end();
	 ++iter) {
      result.push_back(*iter);
    }
    return result;
  }
  
  inline std::vector<parameter_type> IntegerSet::convert_to_vector_of_parameter_types() const {
    std::vector<parameter_type> result;
    result.reserve(card());
    for (const_iterator iter = begin();
	 iter != end();
	 ++iter) {
      result.push_back(*iter);
    }
    return result;
  }

  inline const bool IntegerSet::empty() const {
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
    return (_current_block < _container->_no_of_blocks) && (_current_bit < block_len);
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

}; // namespace topcom

#endif
// eof IntegerSet.hh
