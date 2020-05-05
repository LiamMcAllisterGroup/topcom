////////////////////////////////////////////////////////////////////////////////
// 
// Admissibles.cc
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
#include "Admissibles.hh"

Admissibles::Admissibles(const Circuits& circuits, const Chirotope& chiro, const bool only_empty_simplices) : 
  admissibles_data() {
  size_type count(0);
  const size_type no(circuits.no());
  const size_type rank(circuits.rank());
  SimplicialComplex universe;
  Permutation perm(no, rank);
  do {
    Simplex simp(perm);
    if (chiro[simp] != 0) {
      bool is_empty(true);
      if (only_empty_simplices) {
	for (Circuits::const_iterator iter = circuits.begin();
	     iter != circuits.end();
	     ++iter) {
	  const Circuit circuit(iter->data());
	  if ( (simp.superset(circuit.first) && (circuit.second.card() == 1))
	       || (simp.superset(circuit.second) && (circuit.first.card() == 1)) ) {
	    is_empty = false;
	    break;
	  }
	}
      }
      if (is_empty) {
	universe += simp;
      }
    }
  } while (perm.lexnext());
  if (CommandlineOptions::verbose()) {
    std::cerr << universe.card() << " valid simplices." << std::endl;
  }
  VertexFacetTable vertexfacets(universe);
  for (SimplicialComplex::iterator sciter = universe.begin(); sciter != universe.end(); ++sciter) {
    const Simplex simp(*sciter);
    SimplicialComplex simp_admissibles(universe);
    for (Circuits::const_iterator iter = circuits.begin();
	 iter != circuits.end();
	 ++iter) {
      const Circuit circuit(iter->data());
      if (circuit.first.subset(simp)) {
	SimplicialComplex inadmissibles(vertexfacets.star(circuit.second));
	simp_admissibles -= inadmissibles;
	simp_admissibles -= simp;
      }
      if (circuit.second.subset(simp)) {
	SimplicialComplex inadmissibles(vertexfacets.star(circuit.first));
	simp_admissibles -= inadmissibles;
	simp_admissibles -= simp;
      }
    }
    (*this)[simp] = simp_admissibles;
    if (CommandlineOptions::verbose() && (++count % 100 == 0)) {
      std::cerr << count << " simplices processed so far." << std::endl;
    }
  }
}

// eof Admissibles.cc
