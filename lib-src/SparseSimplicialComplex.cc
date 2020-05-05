////////////////////////////////////////////////////////////////////////////////
// 
// SparseSimplicialComplex.cc
//
//    produced: 29/05/98 jr
// last change: 24/01/99 jr
//
////////////////////////////////////////////////////////////////////////////////

#include "SparseSimplicialComplex.hh"
#include "FastSimplicialComplex.hh"
#include "SimplicialComplexTemplate_Definitions.hh"

// explicit instantiation of members:
// disabled because of MacOSX-gcc problems:
// template SimplicialComplexTemplate<SparseIntegerSet>;

// constructors for conversion:

SparseSimplicialComplex::SparseSimplicialComplex(const FastSimplicialComplex& sc) :
  _SparseSimplicialComplex() {
  for (FastSimplicialComplex::const_iterator iter = sc.begin(); iter != sc.end(); ++iter) {
    insert(*iter);
  }  
}

// eof SparseSimplicialComplex.cc
