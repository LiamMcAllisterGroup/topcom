////////////////////////////////////////////////////////////////////////////////
// 
// FastSimplicialComplex.hh 
//
//    produced: 21/08/97 jr
// last change: 24/01/98 jr
//
////////////////////////////////////////////////////////////////////////////////
#ifndef FASTSIMPLICIALCOMPLEX_HH
#define FASTSIMPLICIALCOMPLEX_HH

#include "SimplicialComplexTemplate_Declarations.hh"
#include "SparseIntegerSet.hh"

#ifdef COMPRESSED_INTEGERSET
#include "CompressedIntegerSet.hh"
typedef SimplicialComplexTemplate<CompressedIntegerSet>       _FastSimplicialComplex;
#else
#include "IntegerSet.hh"
typedef SimplicialComplexTemplate<IntegerSet>                 _FastSimplicialComplex;
#endif

class SparseSimplicialComplex;

class FastSimplicialComplex : public _FastSimplicialComplex {
public:
  // constructors:
  inline FastSimplicialComplex()                                       : _FastSimplicialComplex()        {}
  inline FastSimplicialComplex(const FastSimplicialComplex& sc)        : _FastSimplicialComplex(sc)      {}
  inline FastSimplicialComplex(const _FastSimplicialComplex& sc)       : _FastSimplicialComplex(sc)      {}
  inline FastSimplicialComplex(const Simplex& s, const size_type card) : _FastSimplicialComplex(s, card) {}
  inline FastSimplicialComplex(const Simplex& s)                       : _FastSimplicialComplex(s)       {}
  // additional constructor:
  FastSimplicialComplex(const SparseSimplicialComplex&);
  // destructor:
  inline ~FastSimplicialComplex() {}
  // assignment:
  inline FastSimplicialComplex& operator=(const FastSimplicialComplex& sc) {
    _FastSimplicialComplex::operator=(sc);
    return *this;
  }
};

#endif

// eof FastSimplicialComplex.hh
