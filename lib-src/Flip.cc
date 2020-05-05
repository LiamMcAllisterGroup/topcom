////////////////////////////////////////////////////////////////////////////////
// 
// Flip.cc
//
//    produced: 18/06/98 jr
// last change: 18/06/98 jr
// 
////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <assert.h>

#include "Circuits.hh"
#include "Flip.hh"
#include "TriangNode.hh"

// class FlipRep:
// constructors:
FlipRep::FlipRep(const Chirotope& chiro, const dependent_set_type& ds, const TriangNode& tn) :
  fliprep_type() {

  const Circuit circuit(chiro, ds);

  const Simplex c_support(circuit.support());
  if (c_support.card() == tn.rank() + 1) {
    // general position circuit:
    if (tn.containment_ok(circuit)) {
      first = circuit.first;
      second = circuit.second;
    }
    else if (tn.containment_ok(circuit.inverse())) {
      first = circuit.second;
      second = circuit.first;
    }
  }
  else {
    // special position circuit:
    if (tn.link_ok(circuit)) {
      first = circuit.first;
      second = circuit.second;
    }
    else if (tn.link_ok(circuit.inverse())) {
      first = circuit.second;
      second = circuit.first;
    }
  }
}

// class Flip:
// internal algorithms:
void Flip::_construct(const TriangNode& tn, const Circuit& circuit) {
  Simplex simp(circuit.support());
  // general position circuit:
  if (simp.card() == tn.rank() + 1) {
    for (IntegerSet::const_iterator iter = circuit.first.begin();
	 iter != circuit.first.end();
	 ++iter) {
      simp -= *iter;
      first += simp;
      simp += *iter;
    }
    for (IntegerSet::const_iterator iter = circuit.second.begin();
	 iter != circuit.second.end();
	 ++iter) {
      simp -= *iter;
      second += simp;
      simp += *iter;
    }
  }
  else {
    // special position circuit:
    PlainArray<Simplex> common_link;
    IntegerSet::const_iterator first_iter = circuit.first.begin();
    simp -= *first_iter;
    for (SimplicialComplex::const_iterator tn_iter = tn.begin();
	 tn_iter != tn.end();
	 ++tn_iter) {
      if (tn_iter->superset(simp)) {
	first += *tn_iter;
	common_link.append(*tn_iter - simp);
      }
    }
    simp += *first_iter;
    while (++first_iter != circuit.first.end()) {
      simp -= *first_iter;
      for (size_type i = 0; i < common_link.maxindex(); ++i) {
	first += (simp + common_link[i]);
      }
      simp += *first_iter;
    }
    for (IntegerSet::const_iterator second_iter = circuit.second.begin();
	 second_iter != circuit.second.end();
	 ++second_iter) {
      simp -= *second_iter;
      for (size_type i = 0; i < common_link.maxindex(); ++i) {
	second += (simp + common_link[i]);
      }
      simp += *second_iter;
    }
  }
}

void Flip::_construct(const Chirotope& chiro, const TriangNode& tn, const dependent_set_type& ds) {
  const Circuit circuit(chiro, ds);
  _construct(tn, circuit);
}

// eof Flip.cc
