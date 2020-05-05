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

class CheckTriang {
private:
  const SimplicialComplex& _triang;
  const SymmetryGroup      _triang_symmetries;
  const bool               _fine_only;
  const Chirotope*         _chiroptr;
  const SymmetryGroup*     _symptr;
private:
  CheckTriang();
  CheckTriang(const CheckTriang&);
public:
  inline CheckTriang(const SimplicialComplex& triang, 
		     const SymmetryGroup& triang_symmetries,
		     const Chirotope& chiro,
		     const SymmetryGroup& symmetries,
		     const bool fine_only = false) :
    _triang(triang), 
    _triang_symmetries(triang_symmetries),
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

#endif
// eof CheckTriang.hh
