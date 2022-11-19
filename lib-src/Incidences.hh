////////////////////////////////////////////////////////////////////////////////
// 
// Incidences.hh 
//
//    produced: 19/02/2020 jr
// last change: 19/01/2020 jr
//
////////////////////////////////////////////////////////////////////////////////
#ifndef INCIDENCES_HH
#define INCIDENCES_HH

#include <assert.h>
#include <iostream>

#include "ContainerIO.hh"

#include "SimplicialComplex.hh"

#include "CommandlineOptions.hh"
#include "Chirotope.hh"

#include "Facets.hh"

namespace topcom {

#ifndef STL_CONTAINERS
#include "HashMap.hh"
  typedef HashMap<Simplex, SimplicialComplex>                            incidences_data;
#else
#include <unordered_map>
  typedef std::unordered_map<Simplex, SimplicialComplex, Hash<Simplex> >    simpfacets_incidences_data;
  typedef std::unordered_map<Simplex, facets_data, Hash<Simplex> >          conffacets_incidences_data;
  typedef std::unordered_map<Simplex, SimplicialComplex, Hash<Simplex> >    simpcofacets_incidences_data;
  typedef std::unordered_map<facet_type, SimplicialComplex, Hash<Simplex> > confcofacets_incidences_data;
#endif

  class Incidences {
  private:
    const Chirotope* _chiroptr;
    simpfacets_incidences_data   _intfacets;    // maps each simplex to its interior facets
    conffacets_incidences_data   _conffacets;   // maps each simplex to the configuration facets containing its exterior facets
    simpcofacets_incidences_data _intcofacets;  // maps each interior facet to all simplices it contains
    confcofacets_incidences_data _confcofacets; // maps each facet of the configuration to all simplices with a facet in it
  private:
    Incidences();  
  public:
    // constructors:
    inline Incidences(const Chirotope&);
    inline Incidences(const Incidences&);
    Incidences(const Chirotope&, const Facets&);
    // destructor:
    inline ~Incidences();
    // assignment:
    inline const Incidences& operator=(const Incidences& incidences);
    // accessors:
    inline const simpfacets_incidences_data&   intfacets   ()               const;
    inline const conffacets_incidences_data&   conffacets  ()               const;
    inline const simpcofacets_incidences_data& intcofacets ()               const;
    inline const confcofacets_incidences_data& confcofacets()               const;
    inline const SimplicialComplex& intfacets   (const Simplex&) const;
    inline const facets_data& conffacets  (const Simplex&) const;
    inline const SimplicialComplex& intcofacets (const Simplex&) const;
    inline const SimplicialComplex& confcofacets(const facet_type&) const;
    inline std::istream& read(std::istream&);
    inline std::ostream& write(std::ostream&) const;
    inline friend std::istream& operator<<(std::istream& ist, Incidences&);
    inline friend std::ostream& operator<<(std::ostream& ost, const Incidences&);
  };

  // constructors:
  inline Incidences::Incidences(const Chirotope& chiro) :
    _chiroptr    (&chiro),
    _intfacets   (),
    _conffacets  (),
    _intcofacets (),
    _confcofacets() {}
  inline Incidences::Incidences(const Incidences& incidences) :
    _chiroptr    (incidences._chiroptr),
    _intfacets   (incidences._intfacets),
    _conffacets  (incidences._conffacets),
    _intcofacets (incidences._intcofacets),
    _confcofacets(incidences._confcofacets) {}
  // destructor:
  inline Incidences::~Incidences() {
#ifdef CONSTRUCTOR_DEBUG
    std::cerr << "Incidences::~Incidences(): destructor called." << std::endl;
#endif
  }
  // assignment:
  inline const Incidences& Incidences::operator=(const Incidences& incidences) {
    if (this == &incidences) {
      return *this;
    }
    _chiroptr     = incidences._chiroptr;
    _intfacets    = incidences._intfacets;
    _conffacets   = incidences._conffacets;
    _intcofacets  = incidences._intcofacets;
    _confcofacets = incidences._confcofacets;
    return *this;
  }
  // accessors:
  inline const simpfacets_incidences_data& Incidences::intfacets() const {
    return _intfacets;
  }
  inline const conffacets_incidences_data& Incidences::conffacets() const {
    return _conffacets;
  }
  inline const simpcofacets_incidences_data& Incidences::intcofacets() const {
    return _intcofacets;
  }
  inline const confcofacets_incidences_data& Incidences::confcofacets() const {
    return _confcofacets;
  }
  inline const SimplicialComplex& Incidences::intfacets(const Simplex& simp) const {
#ifndef STL_CONTAINERS
    return _intfacets[simp];
#else
    return _intfacets.find(simp)->second;
#endif
  }
  inline const facets_data& Incidences::conffacets(const Simplex& simp) const {
#ifndef STL_CONTAINERS
    return _conffacets[simp];
#else
    return _conffacets.find(simp)->second;
#endif
  }
  inline const SimplicialComplex& Incidences::intcofacets(const Simplex& simpfacet) const {
#ifndef STL_CONTAINERS
    return _intcofacets[simpfacet];
#else
    return _intcofacets.find(simpfacet)->second;
#endif
  }
  inline const SimplicialComplex& Incidences::confcofacets(const facet_type& conffacet) const {
#ifndef STL_CONTAINERS
    return _confcofacets[conffacet];
#else
    return _confcofacets.find(conffacet)->second;
#endif
  }
  inline std::istream& Incidences::read(std::istream& ist) {
    char c;
    ist >> std::ws >> c;
    if (c != ContainerChars::set_start_char) {
#ifdef READ_DEBUG
      std::cerr << "std::istream& Incidences::read(std::istream& ist): "
		<< "missing opening `" << ContainerChars::set_start_char
		<< "' for Incidences." << std::endl;
#endif
      ist.clear(std::ios::failbit);
      return ist;
    }
    ist >> std::ws >> this->_intfacets;
    ist >> std::ws >> c;
    if (c != ContainerChars::delim_char) {
#ifdef READ_DEBUG
      std::cerr << "std::istream& Incidences::read(std::istream& ist): "
		<< "missing opening `" << ContainerChars::delim_char
		<< "' for Incidences." << std::endl;
#endif
      ist.clear(std::ios::failbit);
      return ist;
    }
    ist >> std::ws >> this->_conffacets;
    ist >> std::ws >> c;
    if (c != ContainerChars::delim_char) {
#ifdef READ_DEBUG
      std::cerr << "std::istream& Incidences::read(std::istream& ist): "
		<< "missing opening `" << ContainerChars::delim_char
		<< "' for Incidences." << std::endl;
#endif
      ist.clear(std::ios::failbit);
      return ist;
    }
    ist >> std::ws >> this->_intcofacets;
    ist >> std::ws >> c;
    if (c != ContainerChars::delim_char) {
#ifdef READ_DEBUG
      std::cerr << "std::istream& Incidences::read(std::istream& ist): "
		<< "missing opening `" << ContainerChars::delim_char
		<< "' for Incidences." << std::endl;
#endif
      ist.clear(std::ios::failbit);
      return ist;
    }
    ist >> std::ws >> this->_confcofacets;
    ist >> std::ws >> c;
    if (c != ContainerChars::set_end_char) {
#ifdef READ_DEBUG
      std::cerr << "std::istream& Incidences::read(std::istream& ist): "
		<< "missing closing `" << ContainerChars::set_end_char
		<< "' for Incidences." << std::endl;
#endif
      ist.clear(std::ios::failbit);
      return ist;
    }
    return ist;
  }
  inline std::ostream& Incidences::write(std::ostream& ost) const {
    ost << ContainerChars::set_start_char;
    ost << this->_intfacets;
    ost << ContainerChars::delim_char;
    ost << this->_conffacets;
    ost << ContainerChars::delim_char;
    ost << this->_intcofacets;
    ost << ContainerChars::delim_char;
    ost << this->_confcofacets;
    ost << ContainerChars::set_end_char;
    return ost;
  }
  inline std::istream& operator>>(std::istream& ist, Incidences& incidences) {
    return incidences.read(ist);
  }
  inline std::ostream& operator<<(std::ostream& ost, const Incidences& incidences) {
    return incidences.write(ost);
  }

}; // namespace topcom

#endif

// eof Incidences.hh
