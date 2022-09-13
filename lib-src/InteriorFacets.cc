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

#include "Field.hh"
#include "Chirotope.hh"
#include "Permutation.hh"
#include "InteriorFacets.hh"

namespace topcom {

  InteriorFacets::InteriorFacets(const Chirotope& chiro, const Facets& facets) :
    _chiroptr(&chiro),
    _cofaces(),
    interiorfacets_data() {
    size_type count(0);
    const size_type no(facets.no());
    const size_type rank(facets.rank());
    SimplicialComplex perm_interiors;
    Permutation perm(no, rank);
    if (CommandlineOptions::debug()) {
      std::cerr << "computing interior facets ..." << std::endl;
    }
    do {
      Simplex simp(perm);
      if ((*_chiroptr)[simp] == FieldConstants::ZERO) {
	continue;
      }
      if (CommandlineOptions::debug()) {
	std::cerr << "computing interior facets of " << simp << " in " << facets << " ..." << std::endl;
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
      if (perm_interiors.empty()) {
	std::cerr << "InteriorFacets::InteriorFacets(const Facets&): "
		  << "no interior facet for " << simp << std::endl;
      }
#endif
      (*this)[simp] = perm_interiors;
      perm_interiors.clear();
      if (CommandlineOptions::verbose() && (++count % CommandlineOptions::report_frequency() == 0)) {
	std::cerr << count << " simplices processed so far." << std::endl;
      }
    } while (perm.lexnext());
    if (CommandlineOptions::debug()) {
      std::cerr << "... done computation of interior facets." << std::endl;
    }
    // store the reverse map in _cofaces:
    for (interiorfacets_data::const_iterator ifiter = this->begin();
	 ifiter != this->end();
	 ++ifiter) {
#ifndef STL_CONTAINERS
      const Simplex& simp(ifiter->key());
      const SimplicialComplex& sc(ifiter->data());
#else
      const Simplex simp(ifiter->first);
      const SimplicialComplex sc(ifiter->second);
#endif
      for (SimplicialComplex::const_iterator sciter = sc.begin();
	   sciter != sc.end();
	   ++sciter) {
	const Simplex facet(*sciter);
	interiorfacets_data::iterator finditer = _cofaces.find(facet);
	if (CommandlineOptions::debug()) {
	  std::cerr << "mapping interior facet " << facet << " to simplex " << simp << std::endl;
	}
	if (finditer == _cofaces.end()) {
#ifndef STL_CONTAINERS
	  _cofaces.insert(facet, simp);
#else
	  _cofaces.insert(std::pair<Simplex, Simplex>(facet, simp));
#endif
	}
	else {
#ifndef STL_CONTAINERS
	  finditer->data() += simp;
#else
	  finditer->second += simp;
#endif
	}
      }
    }
    if (CommandlineOptions::debug()) {
      std::cerr << "...done computation of cofaces map." << std::endl;
    }
    if (CommandlineOptions::debug()) {
      std::cerr << "... done; table: " << *this << std::endl;
    }
  }

}; // namespace topcom

// eof InteriorFacets.cc
