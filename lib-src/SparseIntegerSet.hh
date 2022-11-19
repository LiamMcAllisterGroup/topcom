////////////////////////////////////////////////////////////////////////////////
// 
// SparseIntegerSet.hh
//
//    produced: 19/03/98 jr
// last change: 24/01/99 jr
// 
////////////////////////////////////////////////////////////////////////////////
#ifndef SPARSEINTEGERSET_HH
#define SPARSEINTEGERSET_HH

#include <stdlib.h>

#include "Global.hh"
#include "IntegerSet.hh"

namespace topcom {
  typedef size_type integer_key;
};

#ifndef STL_CONTAINERS
#include "Array.hh"
#include "HashSet.hh"
namespace topcom {
  typedef HashSet<integer_key>  set_data;
};
#else
#include <unordered_set>
#include "ContainerIO.hh"
namespace topcom {
  typedef std::unordered_set<integer_key, Hash<integer_key> > set_data;
};
#endif

namespace topcom {

  class IntegerSet;

  class SparseIntegerSet : public set_data {
  public:
    // constructors:
    inline SparseIntegerSet();					          // constructor for empty set
    inline SparseIntegerSet(const SparseIntegerSet&);                       // copy constructor
    SparseIntegerSet(const IntegerSet&);                                    // constructor for IntegerSet
    inline explicit SparseIntegerSet(const integer_key, const integer_key); // construct set containing the given range
    inline explicit SparseIntegerSet(const integer_key elem);               // constructor for one-element set
#ifndef STL_CONTAINERS
    SparseIntegerSet(const Array<integer_key>&);                            // constructor with given Array of integers
#endif
  
    // destructor:
    inline ~SparseIntegerSet();				                  // destructor

    // functions:
    inline const integer_key card() const;	                          // cardinality

    // boolean functions:
    inline const bool contains(const integer_key) const;  // membership
    const bool superset(const SparseIntegerSet&) const;   // superset relation
    const bool subset(const SparseIntegerSet&) const;     // subset relation
    const bool disjoint(const SparseIntegerSet&) const;   // disjoint relation

    // boolean operators:
    const bool operator==(const SparseIntegerSet&) const; // equality
    const bool operator!=(const SparseIntegerSet&) const; // non-equality
    const bool operator<(const SparseIntegerSet&) const;  // order on intsets
    const bool operator>(const SparseIntegerSet&) const;  // reversed order

    // modifiers: 
    SparseIntegerSet& fill(const integer_key, const integer_key); // fill in elements from start to stop
  
    // adding and deleting an element:
    SparseIntegerSet& operator+=(const integer_key); // add integer
    SparseIntegerSet& operator-=(const integer_key); // delete integer
    SparseIntegerSet& operator^=(const integer_key); // add if not contained/delete if contained

    // union, difference, and intersection with sets:
    SparseIntegerSet& operator+=(const SparseIntegerSet&); // union
    SparseIntegerSet& operator-=(const SparseIntegerSet&); // difference
    SparseIntegerSet& operator*=(const SparseIntegerSet&); // intersection
    SparseIntegerSet& operator^=(const SparseIntegerSet&); // symmetric difference
  
    // returns the cardinalities
    // 0, 1, or 2 (2 or more) of
    // the intersection of several IntegerSet's:
    const size_type intersection_card(const SparseIntegerSet**, 
				      const size_type, 
				      size_type&) const;
    const size_type intersection_nonempty(const SparseIntegerSet**, 
					  const size_type, 
					  size_type&) const;
  
    // the same but a new set is returned:
    SparseIntegerSet operator+(const integer_key)       const; // add integer
    SparseIntegerSet operator-(const integer_key)       const; // delete integer
    SparseIntegerSet operator+(const SparseIntegerSet&) const; // union
    SparseIntegerSet operator-(const SparseIntegerSet&) const; // difference
    SparseIntegerSet operator*(const SparseIntegerSet&) const; // intersection
    SparseIntegerSet operator^(const SparseIntegerSet&) const; // symmetric difference

    // iostream:
    std::istream& read(std::istream&);
    std::ostream& write(std::ostream& ost) const;
    // friends (have to be inlined right here):
    friend std::ostream& operator<<(std::ostream& ost, const SparseIntegerSet& sis) {
      return sis.write(ost);
    }
    friend std::istream& operator>>(std::istream& ist, SparseIntegerSet& sis) {
      return sis.read(ist);
    }
  };

  // SparseIntegerSet inlines:
  // constructors:
  inline SparseIntegerSet::SparseIntegerSet() : set_data() {}
  inline SparseIntegerSet::SparseIntegerSet(const SparseIntegerSet& sis) : set_data(sis) {}
  inline SparseIntegerSet::SparseIntegerSet(const integer_key start, const integer_key stop) : set_data() {
    fill(start, stop);
  }
  inline SparseIntegerSet::SparseIntegerSet(const integer_key elem) : set_data() {
    insert(elem);
  }

  // destructor:
  inline SparseIntegerSet::~SparseIntegerSet() {}

  // functions:
  inline const integer_key SparseIntegerSet::card() const {
    return set_data::size();
  }

  inline const bool SparseIntegerSet::contains(const integer_key elem) const {
    return (find(elem) == end());
  }

}; // namespace topcom

#endif
// eof SparseIntegerSet.hh
