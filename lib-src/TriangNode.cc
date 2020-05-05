////////////////////////////////////////////////////////////////////////////////
// 
// TriangNode.cc
//
//    produced: 19/04/98 jr
// last change: 18/06/98 jr
// 
////////////////////////////////////////////////////////////////////////////////
#include <iostream>

#include <set>

#include "Circuits.hh"
#include "TriangNode.hh"

// functions:

bool TriangNode::containment_ok(const Circuit& circuit) const {
  const Simplex c_support(circuit.support());
  bool ok(true);
  for (Simplex::const_iterator iter = circuit.first.begin();
       iter != circuit.first.end();
       ++iter) {
    const Simplex current(c_support - *iter);
    if (!contains(current)) {
      return false;
    }
  }
  return true;
}

bool TriangNode::link_ok(const Circuit& circuit) const {
  if (circuit.second.is_empty()) {
    return false;
  }
  std::set<Simplex> common_link;
  const Simplex c_support(circuit.support());
  if (c_support.card() == _rank + 1) {
    common_link.insert(Simplex());
    return true;
  }
  Simplex::iterator iter = circuit.first.begin();
  const Simplex current(c_support - *iter);
  for (SimplicialComplex::const_iterator tn_iter = begin();
       tn_iter != end();
       ++tn_iter) {
    if (tn_iter->superset(current)) {
      common_link.insert(*tn_iter - current);
    }
  }
  if (common_link.empty()) {
    return false;
  }
  const size_type card = common_link.size();
  while (++iter != circuit.first.end()) {
    const Simplex next(c_support - *iter);
    size_type count(0);
    for (SimplicialComplex::const_iterator tn_iter = begin();
	 tn_iter != end();
	 ++tn_iter) {
      if (tn_iter->superset(next)) {
	const Simplex link_elem(*tn_iter - next);
	if (common_link.find(link_elem) == common_link.end()) {
	  return false;
	}
	++count;
      }
    }
    if (count != card) {
      return false;
    }
  }
  return true;
}

// eof TriangNode.cc
