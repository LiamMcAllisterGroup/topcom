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

#include "LabelSet.hh"
#include "SimplicialComplex.hh"

#include "CommandlineOptions.hh"

#include "PointConfiguration.hh"
#include "Chirotope.hh"

namespace topcom {  
  typedef LabelSet                         dependent_set_type;
};

#ifndef STL_CONTAINERS
#include "Pair.hh"
namespace topcom {  
  typedef Pair<LabelSet, LabelSet>         circuit_data;
};
#else
#include "ContainerIO.hh"
namespace topcom {  
  typedef std::pair<LabelSet, LabelSet>    circuit_data;
};
#endif

namespace topcom {  

  class Circuit : public circuit_data {
  public:
    // constructors:
    inline Circuit();
    inline Circuit(const Circuit&);
    inline Circuit(Circuit&&);
    Circuit(const Chirotope&, const dependent_set_type&);
    // destructor:
    inline ~Circuit();
    // assignment:
    inline Circuit& operator=(const Circuit&);
    inline Circuit& operator=(Circuit&&);
    // functions:
    inline dependent_set_type support()      const;
    SimplicialComplex  upper_facets() const;
    SimplicialComplex  lower_facets() const;
    inline Circuit     inverse     () const;
  };

  // constructors:
  inline Circuit::Circuit() : circuit_data() {}
  inline Circuit::Circuit(const Circuit& circ) : circuit_data(circ) {}
  inline Circuit::Circuit(Circuit&& circ) : circuit_data(std::move(circ)) {}
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
  inline Circuit& Circuit::operator=(Circuit&& circ) { 
    if (this == &circ) {
      return *this;
    }
    circuit_data::operator=(std::move(circ));
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

#ifndef STL_CONTAINERS
#include "HashMap.hh"
  typedef HashMap<dependent_set_type, Circuit>                                       circuits_data;
#else
#include <unordered_map>
#include "ContainerIO.hh"
  typedef std::unordered_map<dependent_set_type, Circuit, Hash<dependent_set_type> > circuits_data;
#endif


  class Circuits : public circuits_data {
  private:
    parameter_type  _no;
    parameter_type  _rank;
  public:
    // constructors:
    inline Circuits();
    inline Circuits(const Circuits&);
    inline Circuits(Circuits&&);
    Circuits(const Chirotope&);
    // destructor:
    inline ~Circuits();
    // assignment:
    inline Circuits& operator=(const Circuits&);
    inline Circuits& operator=(Circuits&&);
    // accessors:
    inline const parameter_type no() const;
    inline const parameter_type rank() const;  
    // functions:
    inline const LabelSet& positive_part(const dependent_set_type& dependent) const;
    inline const LabelSet& negative_part(const dependent_set_type& dependent) const;
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
  inline Circuits::Circuits(Circuits&& circuits) : 
    circuits_data(std::move(circuits)), _no(circuits._no), _rank(circuits._rank) {
  }

  // destructor:
  inline Circuits::~Circuits() {}

  // accessors:
  inline const parameter_type Circuits::no()   const { return _no; }
  inline const parameter_type Circuits::rank() const { return _rank; }

  // assignment:
  inline Circuits& Circuits::operator=(const Circuits& circuits) {
    if (this == &circuits) {
      return *this;
    }
    circuits_data::operator=(circuits);
    _no = circuits._no;
    _rank = circuits._rank;
    return *this;
  }

  inline Circuits& Circuits::operator=(Circuits&& circuits) {
    if (this == &circuits) {
      return *this;
    }
    circuits_data::operator=(std::move(circuits));
    _no = circuits._no;
    _rank = circuits._rank;
    return *this;
  }

  // functions:
  inline const LabelSet& Circuits::positive_part(const dependent_set_type& dependent) const {
#ifndef STL_CONTAINERS
    return find(dependent)->data().first;
#else
    return find(dependent)->second.first;
#endif
  }
  inline const LabelSet& Circuits::negative_part(const dependent_set_type& dependent) const {
#ifndef STL_CONTAINERS
    return find(dependent)->data().second;
#else
    return find(dependent)->second.second;
#endif
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

}; // namespace topcom

#endif

// eof Circuits.hh
