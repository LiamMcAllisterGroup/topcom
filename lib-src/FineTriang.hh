//////////////////////////////////////////////////////////////////////////
// FineTriang.hh
// produced: 01 Oct 1999 jr
// last change: 01 Oct 1999 jr
/////////////////////////////////////////////////////////////////////////
#ifndef FINETRIANG_HH
#define FINETRIANG_HH

#include <assert.h>

#include "HashSet.hh"
#include "LabelSet.hh"
#include "SimplicialComplex.hh"

#include "CommandlineOptions.hh"

#include "Permutation.hh"
#include "Chirotope.hh"
#include "PlacingTriang.hh"

namespace topcom {

  class FineTriang : public SimplicialComplex {
  private:
    const Chirotope* _chiroptr;
  public:
    inline FineTriang();
    inline FineTriang(const FineTriang& pt);
    inline FineTriang(const Chirotope& chiro);
    inline ~FineTriang();
    inline FineTriang& operator=(const FineTriang& pt);
  private:
    void _flip_in();
    void _flip_in(LabelSet& not_used, const size_type i);
  };

  inline FineTriang::FineTriang() :
    SimplicialComplex(), _chiroptr(0) {}

  inline FineTriang::FineTriang(const FineTriang& pt) : 
    SimplicialComplex(pt), _chiroptr(pt._chiroptr) {}

  inline FineTriang::FineTriang(const Chirotope& chiro) : 
    SimplicialComplex(PlacingTriang(chiro)), _chiroptr(&chiro) {
    _flip_in();
  }

  inline FineTriang::~FineTriang() {}

  inline FineTriang& FineTriang::operator=(const FineTriang& pt) {
    if (this == &pt) return *this;
    SimplicialComplex::operator=(pt);
    _chiroptr = pt._chiroptr;
    return *this;
  }

};

#endif

// eof FineTriang.hh
