////////////////////////////////////////////////////////////////////////////////
// 
// TriangFlips.cc
//
//    produced: 27/05/98 jr
// last change: 27/05/98 jr
// 
////////////////////////////////////////////////////////////////////////////////
#include <iostream>

#include <set>

#include "Flip.hh"
#include "TriangFlips.hh"

// internal algorithms:

void TriangFlips::_remove_destroyed_flips(const TriangNode& tn, 
					  const Flip& flip,
					  const SymmetryGroup& symmetries) {
  static MarkedFlips old_flips;
  old_flips = _flips;
  for (MarkedFlips::const_iterator iter = old_flips.begin(); iter != old_flips.end(); ++iter) {
#ifndef STL_FLIPS
    const FlipRep fliprep(iter->key());
#else
    const FlipRep fliprep((*iter).first);
#endif
    const Flip old_flip = Flip(tn, fliprep);
    if (!(old_flip.first * flip.first).is_empty()) {
      _flips.erase(fliprep);
    }
  }
}

void TriangFlips::_add_new_flips(const Chirotope&         chiro,
				 const TriangNode&        tn,
				 const SimplicialComplex& restriction,
				 const SymmetryGroup&     symmetries,
				 const SymmetryGroup&     tn_symmetries,
				 const bool               forbid_vertex_removal,
				 const bool               forbid_card_change) {

#ifndef STL_CONTAINERS
  static HashSet<dependent_set_type> dependent_sets;
#else
  static std::set<dependent_set_type>     dependent_sets;
#endif
  static dependent_set_type          dependent_set;
#ifndef STL_CONTAINERS
  static SimplicialComplex           done_set;
#else
  static std::set<Simplex>                done_set;
#endif

  const Simplex tn_support(tn.support());
  const Simplex groundset(Permutation(_no, _no));
  const Simplex missing_vertices(groundset - tn_support);
  // any new flip in tn must contain one of the flipped-in simplices:
  size_type countdown(restriction.card());
  for (SimplicialComplex::const_iterator iter1 = restriction.begin();
       iter1 != restriction.end();
       ++iter1) {
    --countdown;
    const Simplex& simp1(*iter1);
#ifndef STL_CONTAINERS
    if (done_set.contains(simp1, _rank)) {
#else
    if (done_set.find(simp1) != done_set.end()) {
#endif
      // processed already:
      continue;
    }
#ifndef STL_CONTAINERS
    done_set.insert(simp1, _rank);
#else
    done_set.insert(simp1);
#endif
    for (SymmetryGroup::const_iterator sym_iter = tn_symmetries.begin();
	 sym_iter != tn_symmetries.end();
	 ++sym_iter) {
      const Symmetry& g(*sym_iter);
#ifndef STL_CONTAINERS
      done_set.insert(g(simp1), _rank);
#else
      done_set.insert(g(simp1));
#endif
    }
    if (CommandlineOptions::verbose() && (_no > 50)) {
      std::cerr << "... still " << countdown << " simplices to check for flips ..." << std::endl;
    }
    // search for missing interior points:
    for (Simplex::const_iterator missing_iter = missing_vertices.begin();
	 missing_iter != missing_vertices.end();
	 ++missing_iter) {
      dependent_set = simp1 + *missing_iter;
#ifndef STL_CONTAINERS
      if (dependent_sets.member(dependent_set)) {
#else
      if (dependent_sets.find(dependent_set) != dependent_sets.end()) {
#endif
	// processed already:
	continue;
      }
      // try to build a flip from dependent_set:
      const FlipRep fliprep(chiro, dependent_set, tn);
      if (fliprep) {
	// succeeded:
	if (forbid_vertex_removal && fliprep.kills_vertex()) {
	  continue;
	}
	if (forbid_card_change && !fliprep.is_balanced()) {
	  continue;
	}
// 	else if (CommandlineOptions::reduce_points() && !fliprep.kills_vertex()) {
// 	  continue;
// 	}
// 	else if (CommandlineOptions::dont_add_points() && fliprep.adds_vertex()) {
// 	  continue;
// 	}
	// insert flip:
	_flips[fliprep] = false;
	dependent_sets.insert(dependent_set);
	for (SymmetryGroup::const_iterator sym_iter = tn_symmetries.begin();
	     sym_iter != tn_symmetries.end();
	     ++sym_iter) {
	  // insert all equivalent flips:
	  const Symmetry& g(*sym_iter);
	  _flips[g(fliprep)] = false;
	  dependent_sets.insert(g(dependent_set));
	}
      }
      else {
	// save processed dependent set:
	dependent_sets.insert(dependent_set);
	for (SymmetryGroup::const_iterator sym_iter = tn_symmetries.begin();
	     sym_iter != tn_symmetries.end();
	     ++sym_iter) {
	  // save equivalent dependent sets:
	  const Symmetry& g(*sym_iter);
	  dependent_sets.insert(g(dependent_set));
	}
      }      
    }
    int count(0);
    // search for adjacent simplices:
    for (SimplicialComplex::const_iterator iter2 = tn.begin();
	 iter2 != tn.end();
	 ++iter2) {
      const Simplex& simp2(*iter2);
      dependent_set = (simp1 + simp2);
      if (dependent_set.card() != _rank + 1) {
	continue;
      }
      // simp2 is adjacent to simp1:
#ifndef STL_CONTAINERS
      if (dependent_sets.member(dependent_set)) {
#else
      if (dependent_sets.find(dependent_set) != dependent_sets.end()) {
#endif
	// processed already:
	continue;
      }
      // try to build a flip from dependent_set:
      const FlipRep fliprep(chiro, dependent_set, tn);
      if (fliprep) {
	// succeeded:
	if (forbid_vertex_removal && fliprep.kills_vertex()) {
	  continue;
	}
// 	else if (CommandlineOptions::reduce_points() && !fliprep.kills_vertex()) {
// 	  continue;
// 	}
	else if (CommandlineOptions::dont_add_points() && fliprep.adds_vertex()) {
	  continue;
	}
	// insert flip:
	_flips[fliprep] = false;
	dependent_sets.insert(dependent_set);
	for (SymmetryGroup::const_iterator sym_iter = tn_symmetries.begin();
	     sym_iter != tn_symmetries.end();
	     ++sym_iter) {
	  // insert all equivalent flips:
	  const Symmetry& g(*sym_iter);
	  _flips[g(fliprep)] = false;
	  dependent_sets.insert(g(dependent_set));
	}
      }
      else {
	// save processed dependent set:
	dependent_sets.insert(dependent_set);
	for (SymmetryGroup::const_iterator sym_iter = tn_symmetries.begin();
	     sym_iter != tn_symmetries.end();
	     ++sym_iter) {
	  // save equivalent dependent sets:
	  const Symmetry& g(*sym_iter);
	  dependent_sets.insert(g(dependent_set));
	}
      }
      if (++count > _rank) {
	// we have all neighbors already:
	break;
      }
    }
  }
  dependent_sets.clear();
  done_set.clear();
  if (!forbid_vertex_removal
      && CommandlineOptions::neighborcount() 
#ifndef STL_FLIPS
      && (_flips.load() < _no - _rank)) {
#else
      && (_flips.size() < _no - _rank)) {
#endif
    std::cerr << "triangulation" << std::endl
	 << tn << std::endl
#ifndef STL_FLIPS
 	 << "has only " << _flips.load() << " flips." << std::endl;
#else
	 << "has only " << _flips.size() << " flips." << std::endl;
#endif
  }
}

// eof TriangFlips.cc
