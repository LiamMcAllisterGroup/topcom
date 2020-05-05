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

#include "HashMap.hh"
#include "SimplicialComplex.hh"

#include "CommandlineOptions.hh"

#include "Facets.hh"

typedef HashMap<Simplex, SimplicialComplex> interiorfacets_data;

class InteriorFacets : public interiorfacets_data {
public:
  // constructors:
  inline InteriorFacets();
  inline InteriorFacets(const InteriorFacets&);
  InteriorFacets(const Facets&);
  // destructor:
  inline ~InteriorFacets();
};

// constructors:
inline InteriorFacets::InteriorFacets() : 
  interiorfacets_data() {}
inline InteriorFacets::InteriorFacets(const InteriorFacets& interiorfacets) :
  interiorfacets_data(interiorfacets) {}
// destructor:
inline InteriorFacets::~InteriorFacets() {}

#endif

// eof InteriorFacets.hh
