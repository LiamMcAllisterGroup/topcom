////////////////////////////////////////////////////////////////////////////////
// 
// Facets.cc
//
//    produced: 13/03/98 jr
// last change: 06/07/98 jr
// 
////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <ctype.h>
#include <string.h>

#include "Facets.hh"

Facets::Facets(const Cocircuits& cocircuits) : 
  facets_data(), _no(cocircuits.no()), _rank(cocircuits.rank()) {
  const IntegerSet groundset(0, cocircuits.no());
  for (Cocircuits::const_iterator iter = cocircuits.begin();
       iter != cocircuits.end();
       ++iter) {
    if (iter->dataptr()->first.is_empty()) {
      const IntegerSet& new_facet(groundset - iter->dataptr()->second);
      if (CommandlineOptions::debug()) {
	std::cerr << "insert new facet " << new_facet << " ..." << std::endl;
      }
      insert(new_facet);
      if (CommandlineOptions::debug()) {
	std::cerr << "... done." << std::endl;
      }
    }
    else if (iter->dataptr()->second.is_empty()) {
      const facet_type& new_facet(groundset - iter->dataptr()->first);
      if (CommandlineOptions::debug()) {
	std::cerr << "insert new facet " << new_facet << " ..." << std::endl;
      }
      insert(new_facet);
      if (CommandlineOptions::debug()) {
	std::cerr << "... done." << std::endl;
      }
    }
  }
}

Facets::Facets(const Chirotope& chiro, const SimplicialComplex& bd_triang) : 
  facets_data(), _no(chiro.no()), _rank(chiro.rank()) {
  const Simplex allpoints(0, no());
  size_type count(0);
  for (SimplicialComplex::const_iterator iter = bd_triang.begin();
       iter != bd_triang.end();
       ++iter) {
    Simplex coplanar_set(*iter);

    // the following merges all other coplanar points:
    Cocircuit cocircuit(chiro, coplanar_set);
    
    // all cocircuits spanned by boundary simplices are either positive or negative:
    Simplex newfacet(allpoints - cocircuit.first - cocircuit.second);
    if (CommandlineOptions::debug()) {
      std::cerr << "new facet: " << newfacet << " from cocircuit " << cocircuit << std::endl;
    }
    facets_data::insert(newfacet);
  }
  if (CommandlineOptions::verbose()) {
    std::cerr << card() << " facets in total." << std::endl;
  }
}

std::ostream& Facets::write(std::ostream& ost) const {
  ost << _no << ',' << _rank << ':' << std::endl;
  ost << '{' << std::endl;
  for (iterator iter = begin(); iter != end(); ++iter) {
    ost << *iter << '\n';
  }
  ost << '}' << std::endl;
  return ost;
}

std::istream& Facets::read(std::istream& ist) {
  char c;

  clear();
  if (!(ist >> std::ws >> _no)) {
#ifdef READ_DEBUG
    std::cerr << "Facets::read_string(std::istream&): "
	 << "number of points not found." << std::endl;
#endif
    ist.clear(std::ios::failbit);
    return ist;
  }
  if (!(ist >> std::ws >> c)) {
#ifdef READ_DEBUG
    std::cerr << "Facets::read_string(std::istream&): "
	 << "separator not found." << std::endl;
#endif
    ist.clear(std::ios::failbit);
    return ist;
  }
  if (!(ist >> std::ws >> _rank)) {
#ifdef READ_DEBUG
    std::cerr << "Facets::read_string(std::istream&): "
	 << "rank not found." << std::endl;
#endif
    ist.clear(std::ios::failbit);
    return ist;
  }
  if (!(ist >> std::ws >> c)) {
#ifdef READ_DEBUG
    std::cerr << "Facets::read_string(std::istream&): "
	 << "separator not found." << std::endl;
#endif
    ist.clear(std::ios::failbit);
    return ist;
  }
  ist >> (facets_data&)*this;
  return ist;
}

// eof Facets.cc
