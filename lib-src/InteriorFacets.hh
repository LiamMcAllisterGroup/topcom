////////////////////////////////////////////////////////////////////////////////
// 
// InteriorFacets.hh 
//
//    produced: 19/04/98 jr
// last change: 19/04/98 jr
//
////////////////////////////////////////////////////////////////////////////////
#ifndef INTERIORFACETS_HH
#define INTERIORFACETS_HH

#include <assert.h>
#include <iostream>

#include "SimplicialComplex.hh"

#include "CommandlineOptions.hh"
#include "Chirotope.hh"

#include "Facets.hh"

#ifndef STL_CONTAINERS
#include "HashMap.hh"
namespace topcom {
  typedef HashMap<Simplex, SimplicialComplex>                            interiorfacets_data;
};  
#else
#include <unordered_map>
namespace topcom {
  typedef std::unordered_map<Simplex, SimplicialComplex, Hash<Simplex> > interiorfacets_data;
};
#endif

namespace topcom {

  class InteriorFacets : public interiorfacets_data {
  private:
    const Chirotope* _chiroptr;
    interiorfacets_data _cofaces; // maps each interior facet to all simplices it contains
  private:
    InteriorFacets();  
  public:
    // constructors:
    inline InteriorFacets(const Chirotope&);
    inline InteriorFacets(const InteriorFacets&);
    InteriorFacets(const Chirotope&, const Facets&);
    // destructor:
    inline ~InteriorFacets();
    // assignment:
    inline const InteriorFacets& operator=(const InteriorFacets& interiorfacets);
    // accessors:
    inline const interiorfacets_data& cofaces() const;
    inline const SimplicialComplex& cofaces(const Simplex& simp) const;
    inline std::istream& read(std::istream&);
    inline std::ostream& write(std::ostream&) const;
    inline friend std::istream& operator<<(std::istream& ist, InteriorFacets&);
    inline friend std::ostream& operator<<(std::ostream& ost, const InteriorFacets&);
  };

  // constructors:
  inline InteriorFacets::InteriorFacets(const Chirotope& chiro) :
    _chiroptr(&chiro),
    _cofaces(),
    interiorfacets_data() {}
  inline InteriorFacets::InteriorFacets(const InteriorFacets& interiorfacets) :
    _chiroptr(interiorfacets._chiroptr),
    _cofaces(interiorfacets._cofaces),
    interiorfacets_data(interiorfacets) {}
  // destructor:
  inline InteriorFacets::~InteriorFacets() {}
  // assignment:
  inline const InteriorFacets& InteriorFacets::operator=(const InteriorFacets& interiorfacets) {
    if (this == &interiorfacets) {
      return *this;
    }
    _chiroptr = interiorfacets._chiroptr;
    _cofaces = interiorfacets._cofaces;
    return *this;
  }
  // accessors:
  inline const interiorfacets_data& InteriorFacets::cofaces() const {
    return _cofaces;
  }
  inline const SimplicialComplex& InteriorFacets::cofaces(const Simplex& simp) const {
#ifndef STL_CONTAINERS
    return _cofaces[simp];
#else
    return _cofaces.find(simp)->second;
#endif
  }
  inline std::istream& InteriorFacets::read(std::istream& ist) {
    ist >> (interiorfacets_data&)(*this);
    ist >> std::ws >> this->_cofaces;
    return ist;
  }
  inline std::ostream& InteriorFacets::write(std::ostream& ost) const {
    ost << (interiorfacets_data&)(*this);
    ost << " <<==>> ";
    ost << this->_cofaces;
    return ost;
  }
  inline std::istream& operator>>(std::istream& ist, InteriorFacets& interiorfacets) {
    return interiorfacets.read(ist);
  }
  inline std::ostream& operator<<(std::ostream& ost, const InteriorFacets& interiorfacets) {
    return interiorfacets.write(ost);
  }

}; // namespace topcom
  
#endif

// eof InteriorFacets.hh
