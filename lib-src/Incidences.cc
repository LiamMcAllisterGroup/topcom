////////////////////////////////////////////////////////////////////////////////
// 
// Incidences.cc
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
#include "Incidences.hh"

namespace topcom {

  Incidences::Incidences(const Chirotope& chiro, const Facets& chirofacets) :
    _chiroptr(&chiro),
    _intfacets(),
    _conffacets(),
    _intcofacets(),
    _confcofacets() {
    size_type count(0);
    const size_type no(chirofacets.no());
    const size_type rank(chirofacets.rank());
    SimplicialComplex interior_facets;
    facets_data config_facets;
    Permutation perm(no, rank);
    if (CommandlineOptions::debug()) {
      std::cerr << "computing interior/exterior incidences ..." << std::endl;
    }
    if (CommandlineOptions::debug()) {
      std::cerr << "computing interior/exterior facets ..." << std::endl;
    }
    // store the maps simplex |-> interior/exterior facets:
    do {
      Simplex simp(perm);
      if ((*_chiroptr)[simp] == FieldConstants::ZERO) {
	continue;
      }
      if (CommandlineOptions::debug()) {
	std::cerr << "classifying facets of " << simp << " in " << chirofacets << " ..." << std::endl;
      }
      Simplex facet(simp);
      for (Simplex::iterator iter = simp.begin(); iter != simp.end(); ++iter) {
	facet -= *iter;
	if (CommandlineOptions::debug()) {
	  std::cerr << "checking facet " << facet << " ..." << std::endl;
	}
	bool is_interior(true);
	facet_type config_facet;
	for (Facets::const_iterator facetsiter = chirofacets.begin();
	     facetsiter != chirofacets.end();
	     ++facetsiter) {
	  if (facetsiter->superset(facet)) {

	    // facet is not interior:
	    is_interior = false;
	    config_facet = *facetsiter;
	    break;
	  }
	}
	if (is_interior) {
	  if (CommandlineOptions::debug()) {
	    std::cerr << "... " << facet << " is an interior facet" << std::endl;
	  }
	  interior_facets += facet;
	}
	else {

	  // add the facet of the simplex to interior facets:
	  config_facets.insert(config_facet);
	  if (CommandlineOptions::debug()) {
	    std::cerr << "... " << facet << " is in a configuration facet" << std::endl;
	  }
	}
	if (CommandlineOptions::debug()) {
	  std::cerr << "... done." << std::endl;
	}
	facet += *iter;
      }
#ifdef COMPUTATIONS_DEBUG
      if (interior_facets.empty()) {
	std::cerr << "Incidences::Incidences(const Facets&): "
		  << "no interior facet for " << simp << std::endl;
      }
      if (config_facets.empty()) {
	std::cerr << "Incidences::Incidences(const Facets&): "
		  << "no facet containing a facet of " << simp << std::endl;
      }
#endif
      _intfacets[simp]  = interior_facets;
      _conffacets[simp] = config_facets;
      interior_facets.clear();
      config_facets.clear();
      if (CommandlineOptions::verbose() && (++count % CommandlineOptions::report_frequency() == 0)) {
	std::cerr << count << " simplices processed so far." << std::endl;
      }
    } while (perm.lexnext());
    if (CommandlineOptions::debug()) {
      std::cerr << "... done computing interior/exterior facets" << std::endl;
    }
    // store the reverse maps interior/exterior facet |-> simplex in _intcofacets/_confcofacets:
    for (simpfacets_incidences_data::const_iterator ifiter = _intfacets.begin();
	 ifiter != _intfacets.end();
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
	simpcofacets_incidences_data::iterator finditer = _intcofacets.find(facet);
	if (CommandlineOptions::debug()) {
	  std::cerr << "mapping interior facet " << facet << " to simplex " << simp << std::endl;
	}
	if (finditer == _intcofacets.end()) {
#ifndef STL_CONTAINERS
	  _intcofacets.insert(facet, simp);
#else
	  _intcofacets.insert(std::pair<Simplex, Simplex>(facet, simp));
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
    for (conffacets_incidences_data::const_iterator efiter = _conffacets.begin();
	 efiter != _conffacets.end();
	 ++efiter) {
#ifndef STL_CONTAINERS
      const Simplex& simp(efiter->key());
      const facets_data& facets(efiter->data());
#else
      const Simplex& simp(efiter->first);
      const facets_data& facets(efiter->second);
#endif
      for (facets_data::const_iterator fiter = facets.begin();
	   fiter != facets.end();
	   ++fiter) {
	const facet_type facet(*fiter);
	confcofacets_incidences_data::iterator finditer = _confcofacets.find(facet);
	if (CommandlineOptions::debug()) {
	  std::cerr << "mapping configuration facet " << facet << " to simplex " << simp << std::endl;
	}
	if (finditer == _confcofacets.end()) {
#ifndef STL_CONTAINERS
	  _confcofacets.insert(facet, simp);
#else
	  _confcofacets.insert(std::pair<facet_type, Simplex>(facet, simp));
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
      std::cerr << "... done computing interior/exterior cofacets" << std::endl;
    }
    if (CommandlineOptions::debug()) {
      std::cerr << "... done; table: " << *this << std::endl;
    }
  }

}; // namespace topcom

// eof Incidences.cc
