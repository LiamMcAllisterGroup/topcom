////////////////////////////////////////////////////////////////////////////////
// 
// MarkedFlips.hh 
//
//    produced: 18/06/98 jr
// last change: 18/06/98 jr
//
////////////////////////////////////////////////////////////////////////////////
#ifndef MARKEDFLIPS_HH
#define MARKEDFLIPS_HH

#include <map>

#include "Pair.hh"
#include "HashSet.hh"
#include "IntegerSet.hh"
#include "SimplicialComplex.hh"

#include "CommandlineOptions.hh"

#include "Circuits.hh"
#include "Flip.hh"

#ifndef STL_FLIPS
typedef PlainHashMap<FlipRep, bool> flips_type; // fliprep->[marked/unmarked]
#else
typedef std::map<FlipRep, bool> flips_type; // fliprep->[marked/unmarked]
#endif

class MarkedFlips : public flips_type {
private:
  size_type _no_of_marked;
public:
  // constructors:
  inline MarkedFlips() : flips_type(), _no_of_marked(0) {}
  inline MarkedFlips(const MarkedFlips& f) : flips_type(f), _no_of_marked(f._no_of_marked) {}
  // destructor:
  inline ~MarkedFlips() {}
  // assignment:
  inline MarkedFlips& operator=(const MarkedFlips& f) {
    if (this == &f) {
      return *this;
    }
    flips_type::operator=(f);
    _no_of_marked = f._no_of_marked;
    return *this;
  }

  // accessors:
  inline const size_type no_of_marked() const { return _no_of_marked; }
#ifndef STL_FLIPS
  inline const bool      all_marked()   const { return (_no_of_marked == load()); }
#else
  inline const bool      all_marked()   const { return (_no_of_marked == size()); }
#endif

  // mark/unmark:
  inline size_type mark(const FlipRep& fliprep) {
    iterator finder(find(fliprep));
#ifdef CHECK_MARK
    std::cerr << "marking " << fliprep << " in " << *this << std::endl;
#endif
    if (finder == end()) {
#ifdef CHECK_MARK
      std::cerr << "WARNING: " << fliprep << " not in " << *this << std::endl;
#endif
      return _no_of_marked;
    }
#ifdef CHECK_MARK
    if (!member(fliprep)) {
      std::cerr << "Error in HashMap:  flip " << fliprep << " is not in " << *this << std::endl;
    }
#endif
#ifndef STL_FLIPS
    if (!finder->data()) {
#else
   if (!(*finder).second) {
#endif
#ifndef STL_FLIPS
      finder->data() = true;
#else
      (*finder).second = true;
#endif
      ++_no_of_marked;
    }
#ifdef CHECK_MARK
    else {
      std::cerr << "WARNING: " << fliprep << " already marked in " << *this << std::endl;
    }
#endif
    return _no_of_marked;
  }
  inline void mark_all() {
    for (iterator iter = begin(); iter != end(); ++iter) {
#ifndef STL_FLIPS
      iter->data() = true;
#else
      (*iter).second = true;
#endif
    }
#ifndef STL_FLIPS
    _no_of_marked = load();
#else
    _no_of_marked = size();
#endif
  }
  inline size_type unmark(const FlipRep& fliprep) {
    iterator finder(find(fliprep));
#ifndef STL_FLIPS
    if (finder->data()) {
#else
    if ((*finder).second) {
#endif
#ifndef STL_FLIPS
      finder->data() = false;
#else
      (*finder).second = false;
#endif
      --_no_of_marked;
    }
    return _no_of_marked;
  }
  inline void unmark_all() {
    for (iterator iter = begin(); iter != end(); ++iter) {
#ifndef STL_FLIPS
      iter->data() = false;
#else
      (*iter).second = false;
#endif
    }
    _no_of_marked = 0;
  }

  // stream output/input:
  std::ostream& write(std::ostream& ost) const {
    flips_type::write(ost);
    return ost;
  }
  std::istream& read(std::istream& ist) {
    flips_type::read(ist);
    _no_of_marked = 0;
    
    for (iterator iter = begin(); iter != end(); ++iter) {
#ifndef STL_FLIPS
      if (iter->data()) {
#else
      if ((*iter).second) {;
#endif
	++_no_of_marked;
      }
    }

    return ist;
  }

  friend inline std::ostream& operator<<(std::ostream& ost, const MarkedFlips& mf) {
    return mf.write(ost);
  }
  
  friend inline std::istream& operator>>(std::istream& ist, MarkedFlips& mf) {
    return mf.read(ist);
  }
  
};

#endif

// eof MarkedFlips.hh
