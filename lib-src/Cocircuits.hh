////////////////////////////////////////////////////////////////////////////////
// 
// Cocircuits.hh 
//
//    produced: 12/09/2006 jr
// last change: 12/09/2006 jr
//
////////////////////////////////////////////////////////////////////////////////
#ifndef COCIRCUITS_HH
#define COCIRCUITS_HH

#include <assert.h>

#include "Pair.hh"
#include "Array.hh"
#include "IntegerSet.hh"
#include "SimplicialComplex.hh"
#include "HashMap.hh"

#include "CommandlineOptions.hh"

#include "PointConfiguration.hh"
#include "Chirotope.hh"

typedef IntegerSet                   dependent_set_type;
typedef Pair<IntegerSet, IntegerSet> cocircuit_data;

class Cocircuit : public cocircuit_data {
public:
  // constructors:
  inline Cocircuit();
  inline Cocircuit(const Cocircuit&);
  Cocircuit(const Chirotope&, const IntegerSet&);
  // destructor:
  inline ~Cocircuit();
  // assignment:
  inline Cocircuit& operator=(const Cocircuit&);
  // functions:
  inline dependent_set_type support()      const;
  inline Cocircuit          inverse     () const;
};

// constructors:
inline Cocircuit::Cocircuit() : cocircuit_data() {}
inline Cocircuit::Cocircuit(const Cocircuit& cocirc) : cocircuit_data(cocirc) {}
// destructor:
inline Cocircuit::~Cocircuit() {}
// assignment:
inline Cocircuit& Cocircuit::operator=(const Cocircuit& cocirc) { 
  if (this == &cocirc) {
    return *this;
  }
  cocircuit_data::operator=(cocirc);
  return *this;
}

// functions:
inline dependent_set_type Cocircuit::support() const {
  return (first + second);
}

inline Cocircuit Cocircuit::inverse () const {
  Cocircuit result;
  result.first = second;
  result.second = first;
  return result;
}

typedef HashMap<IntegerSet, Cocircuit> cocircuits_data;

class Cocircuits : public cocircuits_data {
private:
  parameter_type  _no;
  parameter_type  _rank;
public:
  // constructors:
  inline Cocircuits();
  inline Cocircuits(const Cocircuits&);
  Cocircuits(const Chirotope&, const bool only_positive=false);
  // destructor:
  inline ~Cocircuits();
  // accessors:
  inline const parameter_type no() const;
  inline const parameter_type rank() const;  
  // functions:
  inline const IntegerSet& positive_part(const IntegerSet& coplanar) const;
  inline const IntegerSet& negative_part(const IntegerSet& coplanar) const;
  // stream output/input:
  std::ostream& print_string(std::ostream&) const;
  std::istream& read_string(std::istream&);
  inline friend std::ostream& operator<<(std::ostream&, const Cocircuits&);
  inline friend std::istream& operator>>(std::istream&, Cocircuits&);
};

// constructors:
inline Cocircuits::Cocircuits() : cocircuits_data(), _no(0), _rank(0) {
}
inline Cocircuits::Cocircuits(const Cocircuits& cocircuits) : 
  cocircuits_data(cocircuits), _no(cocircuits._no), _rank(cocircuits._rank) {
}

// destructor:
inline Cocircuits::~Cocircuits() {}

// accessors:
inline const parameter_type Cocircuits::no()   const { return _no; }
inline const parameter_type Cocircuits::rank() const { return _rank; }

// functions:
inline const IntegerSet& Cocircuits::positive_part(const IntegerSet& coplanar) const {
  return member(coplanar)->first;
}
inline const IntegerSet& Cocircuits::negative_part(const IntegerSet& coplanar) const {
  return member(coplanar)->second;
}
// stream output/input:
inline std::ostream& operator<<(std::ostream& ost, const Cocircuits& cocircuits) {
  ost << cocircuits._no << ',' << cocircuits._rank << ':' 
      << (cocircuits_data&)(cocircuits);
  return ost;
}
inline std::istream& operator>>(std::istream& ist, Cocircuits& cocircuits) {
  char c;

  ist >> std::ws >> cocircuits._no >> std::ws >> c >> std::ws >> cocircuits._rank >> std::ws >> c >> std::ws 
      >> (cocircuits_data&)(cocircuits);
  return ist;
}

// typedef Circuit                                  Cocircuit;
// typedef Circuits                                 Cocircuits;

#endif

// eof Cocircuits.hh
