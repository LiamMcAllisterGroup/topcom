////////////////////////////////////////////////////////////////////////////////
// 
// PlacingTriang.cc
//
//    produced: 07/07/98 jr
// last change: 07/07/98 jr
// 
////////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>

#include "Permutation.hh"
#include "Chirotope.hh"
#include "PlacingTriang.hh"
  
void PlacingTriang::_place() {

  const size_type no(_chiroptr->no());
  const size_type rank(_chiroptr->rank());
  if (CommandlineOptions::verbose() && (no > 50)) {
    std::cerr << "searching for non-degenerate start basis ..." << std::endl;
  }
  Simplex start = _chiroptr->find_non_deg_basis();
//   // try a random start basis:
//   srand48(12345);
//   //   do {
//   while (start.card() < rank) {
//     start += (lrand48() % no);
//   }
//   //   } while ((*_chiroptr)(start) == 0);
//   if ((*_chiroptr)(start) == 0) {
//     // search all:
//     Permutation basis_perm(no, rank);
//     do {
//       start = Simplex(basis_perm);
//       if ((*_chiroptr)(start) != 0) {
// 	break;
//       }
//     } while (basis_perm.lexnext());
//   }
  if (CommandlineOptions::verbose() && (no > 50)) {
    std::cerr << std::endl << "... done." << std::endl;
  }
  *this += start;
  const IntegerSet groundset(0, _chiroptr->no());
#ifdef SUPER_VERBOSE
  std::cerr << "points: " << groundset << std::endl;
  std::cerr << "first simplex: " << start << std::endl;
#endif
  IntegerSet placed(start);
  IntegerSet not_placed(groundset - start);
  _bd_triang.insert_boundary(start);
  for (IntegerSet::const_iterator iter = not_placed.begin();
       iter != not_placed.end();
       ++iter) {
    if (CommandlineOptions::verbose() && (no > 50)) {
      std::cerr << "placing vertex " << *iter << " ..." << std::endl;
    }
    _place(placed, *iter);
    if (CommandlineOptions::verbose() && (no > 50)) {
      std::cerr << "... done." << std::endl;
    }
  }
}

void PlacingTriang::_place(IntegerSet& placed, 
			   const size_type i) {
#ifdef SUPER_VERBOSE
  std::cerr << "current facets: " << current_facets << std::endl;
#endif
  SimplicialComplex new_facets;
  for (SimplicialComplex::const_iterator iter = _bd_triang.begin();
       iter != _bd_triang.end();
       ++iter) {
    const Simplex facet(*iter);
#ifdef SUPER_VERBOSE
    std::cerr << "current facet: " << facet << std::endl;
#endif
    if (_is_visible_from(placed, facet, i)) {
#ifdef SUPER_VERBOSE
      std::cerr << "is visible from " << i << std::endl;
#endif
      const Simplex new_simp(facet + i);
      *this += new_simp;
      SimplicialComplex new_bd;
      new_bd.insert_boundary(new_simp);
      new_facets ^= new_bd;
    }
  }
  _bd_triang ^= new_facets;
  placed += i;
}

const bool PlacingTriang::_is_visible_from(const IntegerSet placed,
					   const Simplex& facet,
					   const size_type i) const {
  if ((*_chiroptr)[facet + i] == 0) {
    return false;
  }
  SimplicialComplex facet_lk(link(facet));
  size_type p(*facet_lk.begin()->begin());
  Permutation perm_p(facet);
  perm_p.append(p);
  const int perm_p_sign(perm_p.sign());
  const basis_type basis_p(perm_p);
  const int basis_p_sign((*_chiroptr)(basis_p));
  Permutation perm_i(facet);
  perm_i.append(i);
  const int perm_i_sign(perm_i.sign());
  const basis_type basis_i(perm_i);
  const int basis_i_sign((*_chiroptr)(basis_i));
  return (perm_p_sign * basis_p_sign != perm_i_sign * basis_i_sign);
}


// eof PlacingTriang.cc
