////////////////////////////////////////////////////////////////////////////////
// 
// SimplicialComplex.hh 
//
//    produced: 21/08/97 jr
// last change: 24/01/98 jr
//
////////////////////////////////////////////////////////////////////////////////
#ifndef SIMPLICIALCOMPLEX_HH
#define SIMPLICIALCOMPLEX_HH

#include "FastSimplicialComplex.hh"
#include "SparseSimplicialComplex.hh"

#ifdef SPARSE
typedef SparseSimplicialComplex                     SimplicialComplex;
#else
typedef FastSimplicialComplex                       SimplicialComplex;
#endif

#if defined (STL_CONTAINERS) || defined (NEED_STL_HASH)

#include <tr1/unordered_map>
#include <tr1/unordered_set>

namespace std {
  namespace tr1 {
    template<>				
    struct hash<SimplicialComplex> {
      std::size_t operator()(const SimplicialComplex& sc) const {
	std::size_t result;
	for (size_type k = 0; k < sc.keysize(); ++k) {
	  result ^= sc.key(k);
	}
	return result;
      }
    };      
  };
};                                            
#endif

#endif
// eof SimplicialComplex.hh
