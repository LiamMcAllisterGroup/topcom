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

namespace topcom {

#ifdef SPARSE
typedef SparseSimplicialComplex                     SimplicialComplex;
#else
typedef FastSimplicialComplex                       SimplicialComplex;
#endif

};

#endif
// eof SimplicialComplex.hh
