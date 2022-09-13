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

namespace topcom {

  // internal algorithms:

  void TriangFlips::_remove_destroyed_flips(const TriangNode& tn, 
					    const Flip& flip) {
    static MarkedFlips old_flips;
    old_flips = _flips;
    for (MarkedFlips::const_iterator iter = old_flips.begin(); iter != old_flips.end(); ++iter) {
#ifndef STL_FLIPS
      const FlipRep fliprep(iter->key());
#else
      const FlipRep fliprep((*iter).first);
#endif
      const Flip old_flip = Flip(tn, fliprep);
      if (!(old_flip.first * flip.first).empty()) {
	_flips.erase(fliprep);
      }
    }
  }

  void TriangFlips::_add_new_flips(const Chirotope&            chiro,
				   const TriangNode&           tn,
				   const SimplicialComplex&    restriction,
				   const symmetryptr_datapair& tn_symmetryptrs,
				   const bool                  forbid_vertex_removal,
				   const bool                  forbid_card_change) {

#ifndef STL_CONTAINERS
    HashSet<dependent_set_type> dependent_sets;
#else
    std::set<dependent_set_type> dependent_sets;
#endif
#ifndef STL_CONTAINERS
    SimplicialComplex           done_set;
#else
    std::set<Simplex>           done_set;
#endif
    
    if (CommandlineOptions::debug()) {
      std::cerr << "adding flips in " << tn << " restricted to " << restriction << " ..." << std::endl;
    }
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

      if (CommandlineOptions::debug()) {
	std::cerr << "processing flips containing " << simp1 << " ..." << std::endl;
      }
      
#ifndef STL_CONTAINERS
      if (done_set.contains(simp1, _rank)) {
#else
	if (done_set.find(simp1) != done_set.end()) {
#endif
	  // processed already:
	  continue;
#ifndef STL_CONTAINERS
	}
#else
      }
#endif
#ifndef STL_CONTAINERS
      done_set.insert(simp1, _rank);
#else
      done_set.insert(simp1);
#endif
      for (symmetryptr_iterdata::const_iterator sym_iter = tn_symmetryptrs.first.begin();
	   sym_iter != tn_symmetryptrs.first.end();
	   ++sym_iter) {
	const Symmetry& g(**sym_iter);
#ifndef STL_CONTAINERS
	done_set.insert(g.map(simp1), _rank);
#else
	done_set.insert(g.map(simp1));
#endif
      }
      if (CommandlineOptions::verbose() && (_no > 50)) {
	std::cerr << "... still " << countdown << " simplices to check for flips ..." << std::endl;
      }
      // search for missing interior points:
      for (Simplex::const_iterator missing_iter = missing_vertices.begin();
	   missing_iter != missing_vertices.end();
	   ++missing_iter) {
	dependent_set_type dependent_set(simp1 + *missing_iter);
	if (dependent_sets.find(dependent_set) != dependent_sets.end()) {
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
	  for (symmetryptr_iterdata::const_iterator sym_iter = tn_symmetryptrs.first.begin();
	       sym_iter != tn_symmetryptrs.first.end();
	       ++sym_iter) {
	    // insert all equivalent flips:
	    const Symmetry& g(**sym_iter);
	    _flips[g.map(fliprep)] = false;
	    dependent_sets.insert(g.map(dependent_set));
	  }
	}
	else {
	  // save processed dependent set:
	  dependent_sets.insert(dependent_set);
	  for (symmetryptr_iterdata::const_iterator sym_iter = tn_symmetryptrs.first.begin();
	       sym_iter != tn_symmetryptrs.first.end();
	       ++sym_iter) {
	    // save equivalent dependent sets:
	    const Symmetry& g(**sym_iter);
	    dependent_sets.insert(g.map(dependent_set));
	  }
	}      
      }
      int count(0);
      // search for adjacent simplices:
      for (SimplicialComplex::const_iterator iter2 = tn.begin();
	   iter2 != tn.end();
	   ++iter2) {
	const Simplex& simp2(*iter2);
	
	if (CommandlineOptions::debug()) {
	  std::cerr << "\t processing adjacent simplex " << simp2 << " ..." << std::endl;
	}
	
	dependent_set_type dependent_set(simp1 + simp2);
	if (dependent_set.card() != _rank + 1) {
	  continue;
	}
	// simp2 is adjacent to simp1:
	if (dependent_sets.find(dependent_set) != dependent_sets.end()) {

	  // processed already:
	  if (CommandlineOptions::debug()) {
	    std::cerr << "\t ... already processed - continue" << std::endl;
	  }
	  
	  continue;
	}
	// try to build a flip from dependent_set:
	const FlipRep fliprep(chiro, dependent_set, tn);

	if (CommandlineOptions::debug()) {
	  std::cerr << "\t ... induces flip supported on circuit " << fliprep << " ..." << std::endl;
	}

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
	  for (symmetryptr_iterdata::const_iterator sym_iter = tn_symmetryptrs.first.begin();
	       sym_iter != tn_symmetryptrs.first.end();
	       ++sym_iter) {
	    // insert all equivalent flips:
	    const Symmetry& g(**sym_iter);
	    _flips[g.map(fliprep)] = false;
	    dependent_sets.insert(g.map(dependent_set));
	  }
	}
	else {
	  // save processed dependent set:
	  dependent_sets.insert(dependent_set);
	  for (symmetryptr_iterdata::const_iterator sym_iter = tn_symmetryptrs.first.begin();
	       sym_iter != tn_symmetryptrs.first.end();
	       ++sym_iter) {
	    // save equivalent dependent sets:
	    const Symmetry& g(**sym_iter);
	    dependent_sets.insert(g.map(dependent_set));
	  }
	}
	if (++count > _rank) {
	  // we have all neighbors already:
	  break;
	}
      }
    }
    // dependent_sets.clear();
    // done_set.clear();
    if (!forbid_vertex_removal
	&& CommandlineOptions::neighborcount() 
	&& (_flips.size() < _no - _rank)) {
      std::cerr << "triangulation" << std::endl
		<< tn << std::endl
#ifndef STL_FLIPS
		<< "has only " << _flips.size() << " flips." << std::endl;
#else
      << "has only " << _flips.size() << " flips." << std::endl;
#endif
    }
    if (CommandlineOptions::debug()) {
      std::cerr << "... done." << std::endl;
    }
  }

}; // namespace topcom
  
// eof TriangFlips.cc
