////////////////////////////////////////////////////////////////////////////////
// 
// FastSimplicialComplex.cc
//
//    produced: 29/05/98 jr
// last change: 24/01/99 jr
//
////////////////////////////////////////////////////////////////////////////////

#include "FastSimplicialComplex.hh"
#include "SparseSimplicialComplex.hh"
#include "SimplicialComplexTemplate_Definitions.hh"

namespace topcom {

  // explicit instantiation of members:
  // disabled because of MacOSX-gcc problems:
  // template SimplicialComplexTemplate<IntegerSet>;

  // constructors for conversion:
  FastSimplicialComplex::FastSimplicialComplex(const SparseSimplicialComplex& ssc) :
    _FastSimplicialComplex() {
    for (SparseSimplicialComplex::const_iterator iter = ssc.begin(); iter != ssc.end(); ++iter) {
      insert(*iter);
    }
  }

}; // namespace topcom

// eof SimplicialComplex.cc
