////////////////////////////////////////////////////////////////////////////////
// 
// Circuits.hh 
//
//    produced: 21/08/97 jr
// last change: 30/10/97 jr
//
////////////////////////////////////////////////////////////////////////////////
#ifndef CIRCUITS_HH
#define CIRCUITS_HH

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
typedef Pair<IntegerSet, IntegerSet> circuit_data;

class Circuit : public circuit_data {
public:
  // constructors:
  inline Circuit();
  inline Circuit(const Circuit&);
  Circuit(const Chirotope&, const IntegerSet&);
  // destructor:
  inline ~Circuit();
  // assignment:
  inline Circuit& operator=(const Circuit&);
  // functions:
  inline dependent_set_type support()      const;
  SimplicialComplex  upper_facets() const;
  SimplicialComplex  lower_facets() const;
  inline Circuit     inverse     () const;
};

// constructors:
inline Circuit::Circuit() : circuit_data() {}
inline Circuit::Circuit(const Circuit& circ) : circuit_data(circ) {}
// destructor:
inline Circuit::~Circuit() {}
// assignment:
inline Circuit& Circuit::operator=(const Circuit& circ) { 
  if (this == &circ) {
    return *this;
  }
  circuit_data::operator=(circ);
  return *this;
}

// functions:
inline dependent_set_type Circuit::support() const {
  return (first + second);
}

inline Circuit Circuit::inverse () const {
  Circuit result;
  result.first = second;
  result.second = first;
  return result;
}

typedef HashMap<IntegerSet, Circuit> circuits_data;

class Circuits : public circuits_data {
private:
  parameter_type  _no;
  parameter_type  _rank;
public:
  // constructors:
  inline Circuits();
  inline Circuits(const Circuits&);
  Circuits(const Chirotope&);
  // destructor:
  inline ~Circuits();
  // accessors:
  inline const parameter_type no() const;
  inline const parameter_type rank() const;  
  // functions:
  inline const IntegerSet& positive_part(const IntegerSet& dependent) const;
  inline const IntegerSet& negative_part(const IntegerSet& dependent) const;
  // stream output/input:
  std::ostream& print_string(std::ostream&) const;
  std::istream& read_string(std::istream&);
  inline friend std::ostream& operator<<(std::ostream&, const Circuits&);
  inline friend std::istream& operator>>(std::istream&, Circuits&);
};

// constructors:
inline Circuits::Circuits() : circuits_data(), _no(0), _rank(0) {
}
inline Circuits::Circuits(const Circuits& circuits) : 
  circuits_data(circuits), _no(circuits._no), _rank(circuits._rank) {
}

// destructor:
inline Circuits::~Circuits() {}

// accessors:
inline const parameter_type Circuits::no()   const { return _no; }
inline const parameter_type Circuits::rank() const { return _rank; }

// functions:
inline const IntegerSet& Circuits::positive_part(const IntegerSet& dependent) const {
  return member(dependent)->first;
}
inline const IntegerSet& Circuits::negative_part(const IntegerSet& dependent) const {
  return member(dependent)->second;
}
// stream output/input:
inline std::ostream& operator<<(std::ostream& ost, const Circuits& circuits) {
  ost << circuits._no << ',' << circuits._rank << ':' 
      << (circuits_data&)(circuits);
  return ost;
}
inline std::istream& operator>>(std::istream& ist, Circuits& circuits) {
  char c;

  ist >> std::ws >> circuits._no >> std::ws >> c >> std::ws >> circuits._rank >> std::ws >> c >> std::ws 
      >> (circuits_data&)(circuits);
  return ist;
}

// typedef Circuit                                  Cocircuit;
// typedef Circuits                                 Cocircuits;

#endif

// eof Circuits.hh
