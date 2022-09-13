///////////////////////////////////////////////////////////////////////////////
// 
// IntegerSet.hh
//
//    produced: 21/08/97 jr
// last change: 24/01/99 jr
// 
////////////////////////////////////////////////////////////////////////////////
#ifndef INTEGERSET64_HH
#define INTEGERSET64_HH

#include <stdlib.h>
#include <assert.h>

#include <memory>

#include "Global.hh"
#include "SparseIntegerSet.hh"
#include "IntegerSet.hh"

#ifndef STL_CONTAINERS
#include "PlainArray.hh"
#include "Array.hh"
#else
#include <vector>
#endif

namespace topcom {

  class __is64_const_iterator;

  class IntegerSet64 {
  private:
  public:
    friend class CompressedIntegerSet;
  protected:
    block_type          _bitrep;	// bit representation of the set
#ifdef BITCONSTANTS
    static block_type   _bit[64];	// bits as constant masks
    static block_type   _not_bit[64];	// complement bits as constant masks
#endif
#ifndef BIT_LINEAR_SEARCH
    static unsigned char _S_bit_count[256];
    static unsigned char _S_first_one[256];
#endif

  public:
    // constructors:
    IntegerSet64();					      // constructor for empty set
    IntegerSet64(const IntegerSet64&);                        // copy constructor
    IntegerSet64(IntegerSet64&&);                             // move constructor
    IntegerSet64(const SparseIntegerSet&);                    // constructor for SparseIntegerSet
    explicit IntegerSet64(const size_type, const size_type);  // constructor set containing the given range
    explicit IntegerSet64(const size_type elem);              // constructor for one-element set
    IntegerSet64(const size_type len, const size_type* init); // constructor with given integer array
#ifndef STL_CONTAINERS
    IntegerSet64(const Array<parameter_type>&);               // constructor with given Array of integers
    IntegerSet64(const PlainArray<parameter_type>&);          // constructor with given PlainArray of integers
#else
    explicit IntegerSet64(const std::set<parameter_type>&);   // constructor with given std::set of parameter_types
    explicit IntegerSet64(const std::vector<parameter_type>&);// constructor with given std::vector of parameter_types
#endif
    // destructor:
    ~IntegerSet64();				              // destructor
    // keys for containers:
    const size_type   keysize()        const;
    const size_type   key(size_type n) const;

    // accessors:
    inline const size_type no_of_blocks() const;
    inline const size_type memsize() const;
    inline const block_type invariant() const;

    // conversions:
    std::set<size_type>         convert_to_set_of_size_types()         const;
    std::set<parameter_type>    convert_to_set_of_parameter_types()    const;
    std::vector<size_type>      convert_to_vector_of_size_types()      const;
    std::vector<parameter_type> convert_to_vector_of_parameter_types() const;

    // functions:
    const parameter_type card()     const; // cardinality
    const parameter_type max_elem() const; // returns the maximal element (-1 for empty set)

    // boolean functions:
    const bool contains(const size_type)         const; // membership
    const bool superset(const IntegerSet64&)     const; // superset relation
    const bool subset(const IntegerSet64&)       const; // subset relation
    const bool disjoint(const IntegerSet64&)     const; // disjoint relation
    const bool lexsmaller(const IntegerSet64&)   const; // true lexicographic order (may be slower than "<")
    const bool colexgreater(const IntegerSet64&) const; // true colexicographicorder (may be slower than ">")

    const bool empty()                           const; // test for empty set

    // iterators:
    friend class __is64_const_iterator;
    typedef __is64_const_iterator const_iterator;
    typedef const_iterator      iterator; // no non-const iterators

    // iterator functions:
    inline const_iterator begin()                    const; // iterator on first element
    inline const_iterator end()                      const; // iterator past last element
    inline const_iterator find(const size_type elem) const; // sets iterator to point to elem

    // boolean operators:
    const bool operator==(const IntegerSet64&) const; // equality
    const bool operator!=(const IntegerSet64&) const; // non-equality
    const bool operator<(const IntegerSet64&)  const; // order on intsets
    const bool operator>(const IntegerSet64&)  const; // reversed order

    const size_type operator[](const size_type) const; // pseudo-random-access
  
    IntegerSet64& clear();		    	        // erase all
    IntegerSet64& fill(const size_type, const size_type); // fill in elements from start to stop
    IntegerSet64& operator=(const IntegerSet64&);         // copy assignment
    IntegerSet64& operator=(IntegerSet64&&);              // move assignment

    // adding and deleting an element:
    IntegerSet64& operator+=(const size_type); // add integer
    IntegerSet64& operator-=(const size_type); // delete integer
    IntegerSet64& operator^=(const size_type); // add if not contained/delete if contained

    // union, difference, and intersection with sets:
    IntegerSet64& operator+=(const IntegerSet64&); // union
    IntegerSet64& operator-=(const IntegerSet64&); // difference
    IntegerSet64& operator*=(const IntegerSet64&); // intersection
    IntegerSet64& operator^=(const IntegerSet64&); // symmetric difference
  
    // the same but a new set is returned:
    IntegerSet64 operator+(const size_type)   const; // add integer
    IntegerSet64 operator-(const size_type)   const; // delete integer
    IntegerSet64 operator+(const IntegerSet64&) const; // union
    IntegerSet64 operator-(const IntegerSet64&) const; // difference
    IntegerSet64 operator*(const IntegerSet64&) const; // intersection
    IntegerSet64 operator^(const IntegerSet64&) const; // symmetric difference

    // iostream:
    std::istream& read(std::istream&);
    std::ostream& write(std::ostream&) const;
    friend std::istream& operator>>(std::istream& ist, IntegerSet64& is) {
      return is.read(ist);
    }
    friend std::ostream& operator<<(std::ostream& ost, const IntegerSet64& is) {
      return is.write(ost);
    }
  };

  class __is64_const_iterator {
    friend class IntegerSet64;
    friend class CompressedIntegerSet;
    friend class __cis_const_iterator;
  private:
    const IntegerSet64*  _container;
    size_type            _current_bit;
  public:
    inline __is64_const_iterator();
    inline __is64_const_iterator(const __is64_const_iterator& iter);
    __is64_const_iterator(const IntegerSet64& s);
    inline __is64_const_iterator(const IntegerSet64& s, int);
  private:
    inline __is64_const_iterator(const IntegerSet64& s, 
				 const size_type current_bit);
  public:
    inline __is64_const_iterator& operator=(const __is64_const_iterator& iter);
    inline const bool           valid() const;
    inline const bool           operator==(const __is64_const_iterator&) const;
    inline const bool           operator!=(const __is64_const_iterator&) const;
    inline const size_type      operator*() const;
    __is64_const_iterator& operator++();
    inline __is64_const_iterator  operator++(int);
  };

  // IntegerSet64 inlines:

  inline const bool IntegerSet64::operator!=(const IntegerSet64& s) const {
    return (!((*this) == s));
  }

  inline const size_type IntegerSet64::no_of_blocks() const {
    return 1;
  }

  inline const size_type IntegerSet64::memsize() const {
    return 1;
  }

  inline const block_type IntegerSet64::invariant() const {
    return _bitrep;
  }

  inline const size_type IntegerSet64::keysize() const {
    //   return _no_of_blocks;
    return 1;
  }

  inline std::set<size_type> IntegerSet64::convert_to_set_of_size_types() const {
    std::set<size_type> result;
    for (const_iterator iter = begin();
	 iter != end();
	 ++iter) {
      result.insert(*iter);
    }
    return result;
  }
  
  inline std::set<parameter_type> IntegerSet64::convert_to_set_of_parameter_types() const {
    std::set<parameter_type> result;
    for (const_iterator iter = begin();
	 iter != end();
	 ++iter) {
      result.insert(*iter);
    }
    return result;
  }

  inline std::vector<size_type> IntegerSet64::convert_to_vector_of_size_types() const {
    std::vector<size_type> result;
    result.reserve(card());
    for (const_iterator iter = begin();
	 iter != end();
	 ++iter) {
      result.push_back(*iter);
    }
    return result;
  }
  
  inline std::vector<parameter_type> IntegerSet64::convert_to_vector_of_parameter_types() const {
    std::vector<parameter_type> result;
    result.reserve(card());
    for (const_iterator iter = begin();
	 iter != end();
	 ++iter) {
      result.push_back(*iter);
    }
    return result;
  }

  
  inline const size_type IntegerSet64::key(size_type n) const {
    return static_cast<size_type>(_bitrep);
  }

  inline const bool IntegerSet64::empty() const {
    return (_bitrep == 0UL);
  }

  inline IntegerSet64::const_iterator IntegerSet64::begin() const {
    return IntegerSet64::const_iterator(*this);
  }

  inline IntegerSet64::const_iterator IntegerSet64::end() const {
    return IntegerSet64::const_iterator(*this,0);
  }

  inline IntegerSet64::const_iterator IntegerSet64::find(const size_type elem) const {
    return IntegerSet64::const_iterator(*this, elem % block_len);
  }

  // IntegerSet64::const_iterator inlines:
  inline __is64_const_iterator::__is64_const_iterator() : 
    _container(0), 
    _current_bit(0) {
  }

  inline __is64_const_iterator::__is64_const_iterator(const __is64_const_iterator& iter) :
    _container(iter._container),
    _current_bit(iter._current_bit) {
  }


  inline __is64_const_iterator::__is64_const_iterator(const IntegerSet64& s, int) :
    _container(&s), 
    _current_bit(block_len) {}

  inline __is64_const_iterator::__is64_const_iterator(const IntegerSet64& s, 
						      const size_type current_bit) :
    _container(&s), 
    _current_bit(current_bit) {
    assert(_container->contains(current_bit));
  }

  inline __is64_const_iterator& __is64_const_iterator::operator=(const __is64_const_iterator& iter) {
    if (this == &iter) {
      return *this;
    }
    _container = iter._container;
    _current_bit = iter._current_bit;
    return *this;
  }

  inline const bool __is64_const_iterator::valid() const {
    return (_current_bit < block_len);
  }

  inline const bool __is64_const_iterator::operator==(const __is64_const_iterator& iter) const {
    return ((_container == iter._container)
	    && (_current_bit == iter._current_bit));
  }

  inline const bool __is64_const_iterator::operator!=(const __is64_const_iterator& iter) const {
    return !((*this) == iter);
  }

  inline const size_type __is64_const_iterator::operator*() const {
#ifdef INDEX_CHECK
    if (!valid()) {
      std::cerr << "__is64_const_iterator::operator*(): invalid __is64_const_iterator."
		<< std::endl;
      exit(1);
    }
#endif
    return (_current_bit);
  }

  inline __is64_const_iterator __is64_const_iterator::operator++(int) {
    __is64_const_iterator tmp = *this;
    ++(*this);
    return tmp;
  }

  // #if defined (STL_CONTAINERS) || defined (STL_SYMMETRIES) || defined (STL_CHIROTOPE) || defined (NEED_STL_HASH)

  // #include <unordered_map>
  // #include <unordered_set>

  // namespace std {
  //   template<>				
  //   struct hash<IntegerSet64> {
  //     std::size_t operator()(const IntegerSet64& is) const {
  //       long res(0);
  //       for (int i = 0; i < is.keysize(); ++i) {
  // 	res ^= is.key(i);
  //       }
  //       return res;
  //     }
  //   };      
  // };

  // #endif

};

#endif
// eof IntegerSet64.hh
