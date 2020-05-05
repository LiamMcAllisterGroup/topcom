////////////////////////////////////////////////////////////////////////////////
// 
// SparseSimplicialComplex.hh 
//
//    produced: 21/08/97 jr
// last change: 24/01/98 jr
//
////////////////////////////////////////////////////////////////////////////////
#ifndef SPARSESIMPLICIALCOMPLEX_HH
#define SPARSESIMPLICIALCOMPLEX_HH

#include "SimplicialComplexTemplate_Declarations.hh"

#ifdef COMPRESSED_INTEGERSET
#include "CompressedIntegerSet.hh"
typedef SimplicialComplexTemplate<CompressedIntegerSet>       _FastSimplicialComplex;
#else
#include "IntegerSet.hh"
typedef SimplicialComplexTemplate<IntegerSet>                 _FastSimplicialComplex;
#endif

#include "SparseIntegerSet.hh"
typedef SimplicialComplexTemplate<SparseIntegerSet>           _SparseSimplicialComplex;

class FastSimplicialComplex;

class SparseSimplicialComplex : public _SparseSimplicialComplex {
public:
  // constructors:
  inline SparseSimplicialComplex()                                       : _SparseSimplicialComplex()        {}
  inline SparseSimplicialComplex(const SparseSimplicialComplex& sc)      : _SparseSimplicialComplex(sc)      {}
  inline SparseSimplicialComplex(const _SparseSimplicialComplex& sc)     : _SparseSimplicialComplex(sc)      {}
  inline SparseSimplicialComplex(const Simplex& s, const size_type card) : _SparseSimplicialComplex(s, card) {}
  inline SparseSimplicialComplex(const Simplex& s)                       : _SparseSimplicialComplex(s)       {}
  // additional constructor:
  SparseSimplicialComplex(const FastSimplicialComplex&);
  // destructor:
  inline ~SparseSimplicialComplex() {}
  // assignment:
  inline SparseSimplicialComplex& operator=(const SparseSimplicialComplex& sc) {
    _SparseSimplicialComplex::operator=(sc);
    return *this;
  }
};

#endif

// eof SparseSimplicialComplex.hh
