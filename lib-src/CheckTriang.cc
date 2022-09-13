////////////////////////////////////////////////////////////////////////////////
// 
// CheckTriang.cc
//
//    produced: 22 Nov 1999 jr
// last change: 22 Nov 1999 jr
// 
////////////////////////////////////////////////////////////////////////////////

#include "LabelSet.hh"
#include "SimplicialComplex.hh"
#include "Permutation.hh"
#include "Chirotope.hh"

#include "CheckTriang.hh"

#include "CommandlineOptions.hh"

namespace topcom {
  
  const bool CheckTriang::operator()() const {

    const size_type no(_chiroptr->no());
    const size_type rank(_chiroptr->rank());
    const size_type dim(rank - 1);
  
    // first check unique conver of a lexicographic extension:
    if (!_check_extension_cover()) {
      std::cerr << "Triangulation covers an interior extension twice." << std::endl;
      return false;
    }

    // then check intersection of adjacent simplices:
    size_type countdown(_triang.card());
    if (_fine_only && (_triang.support() != LabelSet(0, no))) {
      if (CommandlineOptions::verbose()) {
	std::cerr << "Triangulation does not use all vertices." << std::endl;
      }
      return false;
    }
    //   HashSet<Simplex> done_facets;
    SimplicialComplex done_facets;
    SimplicialComplex done_simplices;
    for (SimplicialComplex::const_iterator iter = _triang.begin();
	 iter != _triang.end();
	 ++iter) {
      --countdown;
      const Simplex simp(*iter);
      if (simp.card() != rank) {
	if (CommandlineOptions::verbose()) {
	  std::cerr << "Triangulation contains simplex with too few vertices." << std::endl;
	}
	return false;
      }
      if (done_simplices.contains(simp, rank)) {
	continue;
      }
      done_simplices.insert(simp, rank);
      for (symmetryptr_iterdata::const_iterator sym_iter = _triang_symmetryptrs.first.begin();
	   sym_iter != _triang_symmetryptrs.first.end();
	   ++sym_iter) {
	const Symmetry& g(**sym_iter);
	done_simplices.insert(g.map(simp), rank);
      }
      Simplex facet(simp);
      if (CommandlineOptions::verbose()) {
	std::cerr << "... still facets of " << countdown << " simplices to check for correctness ..." 
		  << std::endl;
      }
      for (Simplex::const_iterator simp_iter = simp.begin();
	   simp_iter != simp.end();
	   ++simp_iter) {
	facet -= *simp_iter;
	//       if (done_facets.member(facet, dim)) {
	if (done_facets.contains(facet, dim)) {
	  facet += *simp_iter;
	  continue;
	}
	if (!_check(facet)) {
	  return false;
	}
	done_facets.insert(facet, dim);
	for (symmetryptr_iterdata::const_iterator sym_iter = _triang_symmetryptrs.first.begin();
	     sym_iter != _triang_symmetryptrs.first.end();
	     ++sym_iter) {
	  const Symmetry& g(**sym_iter);
	  done_facets.insert(g.map(facet), dim);
	}
	facet += *simp_iter;
      }
    }
    return true;
  }

  const bool CheckTriang::_check(const Simplex& facet) const {
    // we know already that the link of facet contains only vertices:
    Simplex facet_lk;
    size_type facet_lk_card(0);
    for (SimplicialComplex::const_iterator iter = _triang.begin();
	 iter != _triang.end();
	 ++iter) {
      const Simplex simp(*iter);
      if (iter->superset(facet)) {
	facet_lk += simp;
	++facet_lk_card;
      }
    }
    facet_lk -= facet;
    if (facet_lk_card == 0) {
      if (CommandlineOptions::verbose()) {
	std::cerr << facet << " lies in no simplex." << std::endl;
      }
      return false;
    }
    Simplex::const_iterator iter(facet_lk.begin());
    size_type i(*iter);
    if (facet_lk.card() == 1) {
      for (size_type j = 0; j < _chiroptr->no(); ++j) {
	if (_opposite_sides(facet, i, j)) {
	  if (CommandlineOptions::verbose()) {
	    std::cerr << facet << " lies in exactly one simplex "
		      << facet + i << std::endl
		      << "but is not contained in a facet because "
		      << j << " and " << i << " are on opposite sides." << std::endl;
	  }
	  return false;
	}
      }
      return true;
    }
    else if (facet_lk_card == 2) {
      size_type j(*(++iter));
      if (!_opposite_sides(facet, i, j)) {
	if (CommandlineOptions::verbose()) {
	  std::cerr << facet << " lies in two simplices "
		    << facet + i << " and " << facet + j << std::endl
		    << "but " << i << " and " << j
		    << " do not lie on opposite sides." << std::endl;
	}
	return false;
      }
      return true;
    }
    else {
      if (CommandlineOptions::verbose()) {
	std::cerr << facet << " lies in more than two simplices." << std::endl;
      }
      return false;
    }
  }

  const bool CheckTriang::_opposite_sides(const Simplex& facet,
					  const size_type i,
					  const size_type j) const {
    if (facet.contains(i) || facet.contains(j)) {
      return false;
    }
    Permutation perm_i(facet);
    perm_i.push_back(i);
    const int perm_i_sign(perm_i.sign());
    const basis_type basis_i(perm_i);
    const int basis_i_sign((*_chiroptr)(basis_i));
    Permutation perm_j(facet);
    perm_j.push_back(j);
    const int perm_j_sign(perm_j.sign());
    const basis_type basis_j(perm_j);
    const int basis_j_sign((*_chiroptr)(basis_j));
    return (perm_i_sign * basis_i_sign == -perm_j_sign * basis_j_sign);
  }

  const bool CheckTriang::_check_extension_cover() const {
    size_type countdown(_triang.card());
    if (_triang.empty()) {
      return false;
    }
    SimplicialComplex::const_iterator iter(_triang.begin());

    // this is the simplex we check for an interior lexicographic extension in
    // the reference simplex; no other simplex may contain this extension in its
    // convex hull; unfortunately, we cannot make use of the symmetries, 
    // but the check is fast, anyway:
    const Simplex& reference_simp(*iter);
  
    // form the permutation for the lexicographic extension:
    const Permutation extension_lex_perm(_chiroptr->no(), _chiroptr->rank(), reference_simp);
    while (++iter != _triang.end()) {
      if (CommandlineOptions::verbose()) {
	std::cerr << "... still " << --countdown << " simplices to check for unique extension cover ..." 
		  << std::endl;
      }
      const Simplex& checksimp(*iter);
      Simplex facet(checksimp);
      bool checksimp_okay = false;
    
      // check all signs of facets of checksimp plus the extension point:

      int pm((*_chiroptr)(checksimp));
      if (_chiroptr->rank() % 2) {
	pm = -pm;
      }
      for (Simplex::const_iterator simp_iter = checksimp.begin();
	   simp_iter != checksimp.end();
	   ++simp_iter) {
	facet -= *simp_iter;
	if ((*_chiroptr)(facet, extension_lex_perm) == pm) {

	  // checksimp is okay:
	  checksimp_okay = true;
	}
	pm = -pm;
	facet += *simp_iter;
      }
      if (!checksimp_okay) {
	if (CommandlineOptions::verbose()) {
	  std::cerr << "lexicographic extension in " << reference_simp
		    << " is also covered by " << checksimp
		    << std::endl;
	}
	return false;
      }
    }
    return true;
  }

};

// eof CheckTriang.cc
