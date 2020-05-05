////////////////////////////////////////////////////////////////////////////////
// 
// InteriorFacets.cc
//
//    produced: 19/04/98 jr
// last change: 19/04/98 jr
// 
////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <ctype.h>
#include <string.h>

#include "VertexFacetTable.hh"

#include "Permutation.hh"
#include "InteriorFacets.hh"

InteriorFacets::InteriorFacets(const Facets& facets) : 
  interiorfacets_data() {
  size_type count(0);
  const size_type no(facets.no());
  const size_type rank(facets.rank());
  SimplicialComplex perm_interiors;
  Permutation perm(no, rank);
  do {
    Simplex simp(perm);
    if (CommandlineOptions::debug()) {
      std::cerr << "computing interior facets of " << simp << " ..." << std::endl;
    }
    if (facets.contains(simp) || facets.contains_face(simp)) {
      continue;
    }
    Simplex facet(simp);
    for (Simplex::iterator iter = simp.begin(); iter != simp.end(); ++iter) {
      facet -= *iter;
      if (CommandlineOptions::debug()) {
	std::cerr << "checking facet " << facet << " ..." << std::endl;
      }
      if (!facets.contains(facet) && !facets.contains_face(facet)) {
	perm_interiors += facet;
      }
      if (CommandlineOptions::debug()) {
	std::cerr << "... done." << std::endl;
      }
      facet += *iter;
    }
#ifdef COMPUTATIONS_DEBUG
    if (perm_interiors.is_empty()) {
      std::cerr << "InteriorFacets::InteriorFacets(const Facets&): "
	   << "no interior facet for " << simp << std::endl;
    }
#endif
    (*this)[simp] = perm_interiors;
    perm_interiors.clear();
    if (CommandlineOptions::verbose() && (++count % 100 == 0)) {
      std::cerr << count << " simplices processed so far." << std::endl;
    }
  } while (perm.lexnext());
  if (CommandlineOptions::debug()) {
    std::cerr << "... done.  Table: " << *this << std::endl;
  }
}

// eof InteriorFacets.cc
