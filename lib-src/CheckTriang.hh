////////////////////////////////////////////////////////////////////////////////
// 
// CheckTriang.hh 
//
//    produced: 22 Nov 1999 jr
// last change: 22 Nov 1999 jr
//
////////////////////////////////////////////////////////////////////////////////
#ifndef CHECKTRIANG_HH
#define CHECKTRIANG_HH

#include "SimplicialComplex.hh"
#include "Chirotope.hh"
#include "Symmetry.hh"

namespace topcom {
  
  class CheckTriang {
  private:
    const SimplicialComplex&    _triang;
    const symmetryptr_datapair  _triang_symmetryptrs;
    const bool                  _fine_only;
    const Chirotope*            _chiroptr;
    const SymmetryGroup*        _symptr;
  private:
    CheckTriang();
    CheckTriang(const CheckTriang&);
  public:
    inline CheckTriang(const SimplicialComplex& triang, 
		       const symmetryptr_datapair& triang_symmetryptrs,
		       const Chirotope& chiro,
		       const SymmetryGroup& symmetries,
		       const bool fine_only = false) :
      _triang(triang), 
      _triang_symmetryptrs(triang_symmetryptrs),
      _fine_only(fine_only), 
      _chiroptr(&chiro), 
      _symptr(&symmetries) {}
  public:
    const bool operator()() const;
  private:
    const bool _check(const Simplex& facet) const;
    const bool _opposite_sides(const Simplex& facet,
			       const size_type,
			       const size_type) const;
    const bool _check_extension_cover() const;
  };

}; // namespace topcom

#endif
// eof CheckTriang.hh
