////////////////////////////////////////////////////////////////////////////////
// 
// PartialTriang.hh 
//
//    produced: 30/04/98 jr
// last change: 30/04/98 jr
//
////////////////////////////////////////////////////////////////////////////////
#ifndef PARTIALTRIANG_HH
#define PARTIALTRIANG_HH

#include "HashMap.hh"
#include "SimplicialComplex.hh"

#include "CommandlineOptions.hh"

#include "Permutation.hh"
#include "PointConfiguration.hh"
#include "Chirotope.hh"
#include "Circuits.hh"
#include "Facets.hh"
#include "Admissibles.hh"
#include "InteriorFacets.hh"

class PartialTriang : public SimplicialComplex {
private:
  parameter_type           _no;
  parameter_type           _rank;
  const InteriorFacets*    _intfacetsptr;
  const Admissibles*       _admtableptr;
  SimplicialComplex        _admissibles;
  SimplicialComplex        _freefacets;
private:
  PartialTriang();
public:
  // constructors:
  inline PartialTriang(const parameter_type no, 
		       const parameter_type rank, 
		       const Admissibles& admtable, 
		       const InteriorFacets& intfacets);
  inline PartialTriang(const PartialTriang& pt);
  inline PartialTriang(const PartialTriang& pt, const Simplex& new_simp);
  inline PartialTriang(const PartialTriang& pt, 
		       const Simplex& new_simp, 
		       const SimplicialComplex& forbidden);
  // destructor:
  inline ~PartialTriang();
  // assignments:
  inline PartialTriang& operator=(const PartialTriang& pt);
  // accessors:
  inline const parameter_type     no()                 const { return _no; }
  inline const parameter_type     rank()               const { return _rank; }
  inline const InteriorFacets*    intfacetsptr()       const { return _intfacetsptr; }
  inline const Admissibles*       admtableptr()        const { return _admtableptr; }
  inline const SimplicialComplex& admissibles()        const { return _admissibles; }
  inline const SimplicialComplex& freefacets()         const { return _freefacets; }
  // functions:
  inline void forbid(const Simplex&);
  inline void forbid(const SimplicialComplex&);
  inline bool complete();
private:
  // internal algorithms:
  void _add_simplex(const Simplex& simp);
  void _add_simplex(const Simplex& simp, const SimplicialComplex& forbidden);
  void _update_admissibles(const Simplex& simp);
  void _update_admissibles(const Simplex& simp, const SimplicialComplex& forbidden);
  void _update_freefacets(const Simplex& simp);
};

// constructors:
inline PartialTriang::PartialTriang(const parameter_type no, 
				    const parameter_type rank, 
				    const Admissibles& admtable, 
				    const InteriorFacets& intfacets) : 
  SimplicialComplex(), 
  _no(no), _rank(rank), 
  _intfacetsptr(&intfacets), _admtableptr(&admtable), 
  _admissibles(), _freefacets() {
  for (Admissibles::const_iterator iter = _admtableptr->begin(); iter != _admtableptr->end(); ++iter) {
    _admissibles += iter->key();
  }
}
inline PartialTriang::PartialTriang(const PartialTriang& pt) : 
  SimplicialComplex(pt), 
  _no(pt._no), _rank(pt._rank),
  _intfacetsptr(pt._intfacetsptr), _admtableptr(pt._admtableptr), 
  _admissibles(pt._admissibles), _freefacets(pt._freefacets) {}
inline PartialTriang::PartialTriang(const PartialTriang& pt, const Simplex& new_simp) : 
  SimplicialComplex(pt), 
  _no(pt._no), _rank(pt._rank),
  _intfacetsptr(pt._intfacetsptr), _admtableptr(pt._admtableptr), 
  _admissibles(pt._admissibles), _freefacets(pt._freefacets) {
  _add_simplex(new_simp);
}
inline PartialTriang::PartialTriang(const PartialTriang& pt, 
				    const Simplex& new_simp, 
				    const SimplicialComplex& forbidden) : 
  SimplicialComplex(pt), 
  _no(pt._no), _rank(pt._rank),
  _intfacetsptr(pt._intfacetsptr), _admtableptr(pt._admtableptr), 
  _admissibles(pt._admissibles), _freefacets(pt._freefacets) {
  _add_simplex(new_simp, forbidden);
}

// destructor:
inline PartialTriang::~PartialTriang() {}

// assignment:
inline PartialTriang& PartialTriang::operator=(const PartialTriang& pt) {
  if (this == &pt) {
    return *this;
  }
  SimplicialComplex::operator=(pt);
  _no = pt._no;
  _rank = pt._rank;
  _intfacetsptr = pt._intfacetsptr;
  _admtableptr = pt._admtableptr;
  _admissibles = pt._admissibles;
  _freefacets = pt._freefacets;
  return *this;
}

// functions:
inline void PartialTriang::forbid(const Simplex& simp) {
  _admissibles -= simp;
}
inline void PartialTriang::forbid(const SimplicialComplex& sc) {
  _admissibles -= sc;
}
inline bool PartialTriang::complete() {
  if (_freefacets.is_empty()) {
    return true;
  }
  for (SimplicialComplex::iterator iter = _admissibles.begin(); 
       iter != _admissibles.end(); 
       ++iter) {
    PartialTriang new_partialtriang(*this, *iter);
    if (new_partialtriang.complete()) {
      *this = new_partialtriang;
      return true;
    }
  }
  return false;
}

// internal algorithms:
inline void PartialTriang::_add_simplex(const Simplex& simp) {
  *this += simp;
  _update_admissibles(simp);
  _update_freefacets(simp);
}
inline void PartialTriang::_add_simplex(const Simplex& simp, const SimplicialComplex& forbidden) {
  *this += simp;
  _update_admissibles(simp, forbidden);
  _update_freefacets(simp);
}
inline void PartialTriang::_update_admissibles(const Simplex& simp) {
  _admissibles *= (*_admtableptr)[simp];
}
inline void PartialTriang::_update_admissibles(const Simplex& simp, 
					       const SimplicialComplex& forbidden) {
  _admissibles *= (*_admtableptr)[simp];
  _admissibles -= forbidden;
}
inline void PartialTriang::_update_freefacets(const Simplex& simp) {
  _freefacets ^= (*_intfacetsptr)[simp];
}

#endif

// eof PartialTriang.hh
