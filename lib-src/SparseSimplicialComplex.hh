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
namespace topcom {
  typedef SimplicialComplexTemplate<CompressedIntegerSet>       _FastSimplicialComplex;
};
#else
#include "IntegerSet.hh"
namespace topcom {
  typedef SimplicialComplexTemplate<IntegerSet>                 _FastSimplicialComplex;
};
#endif

#include "SparseIntegerSet.hh"
namespace topcom {

  typedef SimplicialComplexTemplate<SparseIntegerSet>           _SparseSimplicialComplex;

  class FastSimplicialComplex;

  class SparseSimplicialComplex : public _SparseSimplicialComplex {
  public:
    // constructors:
    inline SparseSimplicialComplex()                                            : _SparseSimplicialComplex()              {}
    inline SparseSimplicialComplex(const SparseSimplicialComplex& sc)           : _SparseSimplicialComplex(sc)            {}
    inline SparseSimplicialComplex(SparseSimplicialComplex&& sc)                : _SparseSimplicialComplex(std::move(sc)) {}
    inline SparseSimplicialComplex(const _SparseSimplicialComplex& sc)          : _SparseSimplicialComplex(sc)            {}
    inline SparseSimplicialComplex(_SparseSimplicialComplex&& sc)               : _SparseSimplicialComplex(std::move(sc)) {}
    inline SparseSimplicialComplex(const Simplex& s, const parameter_type card) : _SparseSimplicialComplex(s, card)       {}
    inline SparseSimplicialComplex(const Simplex& s)                            : _SparseSimplicialComplex(s)             {}
    // additional constructor:
    SparseSimplicialComplex(const FastSimplicialComplex&);
    // destructor:
    inline ~SparseSimplicialComplex() {}
    // assignment:
    inline SparseSimplicialComplex& operator=(const SparseSimplicialComplex& sc) {
      _SparseSimplicialComplex::operator=(sc);
      return *this;
    }
    inline SparseSimplicialComplex& operator=(SparseSimplicialComplex&& sc) {
      _SparseSimplicialComplex::operator=(std::move(sc));
      return *this;
    }
  };

}; // namespace topcom

#endif

// eof SparseSimplicialComplex.hh
