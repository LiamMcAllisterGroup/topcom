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

#include "SimplicialComplex.hh"
#include "SparseSimplicialComplex.hh"
#include "FastSimplicialComplex.hh"

#include "CommandlineOptions.hh"

#include "Field.hh"
#include "Permutation.hh"
#include "PointConfiguration.hh"
#include "Chirotope.hh"
#include "Circuits.hh"
#include "Facets.hh"
#include "Admissibles.hh"
#include "Incidences.hh"
#include "Volumes.hh"

namespace topcom {

  typedef SimplicialComplex pt_complex_type;

  class PartialTriang : public pt_complex_type {
  private:
    parameter_type           _no;
    parameter_type           _rank;
    const Admissibles*       _admtableptr;
    const Incidences*        _inctableptr;
    const Volumes*           _voltableptr;
    SimplicialComplex        _admissibles;
    SimplicialComplex        _freeintfacets;
    facets_data              _freeconffacets;
    Field                    _covered_volume;
  private:
    PartialTriang();
  public:
    // constructors:
    inline PartialTriang(const parameter_type no, 
			 const parameter_type rank, 
			 const Admissibles&   admtable, 
			 const Incidences&    inctable,
			 const Volumes*       voltableptr);
    inline PartialTriang(const PartialTriang& pt);
    inline PartialTriang(PartialTriang&& pt);
    inline PartialTriang(const PartialTriang& pt, const Simplex& new_simp);
    inline PartialTriang(const PartialTriang& pt, 
			 const Simplex& new_simp, 
			 const SimplicialComplex& forbidden);
    // destructor:
    inline ~PartialTriang();
    // assignments:
    inline PartialTriang& operator=(const PartialTriang& pt);
    inline PartialTriang& operator=(PartialTriang&& pt);
    // accessors:
    inline const parameter_type     no()                 const { return _no; }
    inline const parameter_type     rank()               const { return _rank; }
    inline const Admissibles*       admtableptr()        const { return _admtableptr; }
    inline const Incidences*        inctableptr()        const { return _inctableptr; }
    inline const Volumes*           voltableptr()        const { return _voltableptr; }
    inline const SimplicialComplex& admissibles()        const { return _admissibles; }
    inline const SimplicialComplex& freeintfacets()      const { return _freeintfacets; }
    inline const facets_data&       freeconffacets()     const { return _freeconffacets; }
    inline const Field&             covered_volume()     const { return _covered_volume; }
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
    void _update_volume(const Simplex& simp);
  };

  // constructors:
  inline PartialTriang::PartialTriang(const parameter_type no, 
				      const parameter_type rank, 
				      const Admissibles&   admtable, 
				      const Incidences&    inctable,
				      const Volumes*       voltableptr) : 
    pt_complex_type(), 
    _no(no),
    _rank(rank), 
    _admtableptr(&admtable),
    _inctableptr(&inctable),
    _voltableptr(voltableptr),
    _admissibles(),
    _freeintfacets(),
    _freeconffacets(),
    _covered_volume(FieldConstants::ZERO) {
    for (Admissibles::const_iterator iter = _admtableptr->begin();
	 iter != _admtableptr->end();
	 ++iter) {
#ifndef STL_CONTAINERS
      _admissibles += iter->key();
#else
      _admissibles += iter->first;
#endif
    }
    for (confcofacets_incidences_data::const_iterator iter = _inctableptr->confcofacets().begin();
	 iter != _inctableptr->confcofacets().end();
	 ++iter) {
#ifndef STL_CONTAINERS
      _freeconffacets.insert(iter->key());
#else
      _freeconffacets.insert(iter->first);
#endif
    }
  }
  inline PartialTriang::PartialTriang(const PartialTriang& pt) : 
    pt_complex_type(pt), 
    _no(pt._no),
    _rank(pt._rank),
    _admtableptr(pt._admtableptr),
    _inctableptr(pt._inctableptr),
    _voltableptr(pt._voltableptr),
    _admissibles(pt._admissibles),
    _freeintfacets(pt._freeintfacets),
    _freeconffacets(pt._freeconffacets),
    _covered_volume(pt._covered_volume) {
#ifdef CONSTRUCTOR_DEBUG
    std::cerr << "PartialTriang::PartialTriang(const PartialTriang& pt): copy constructur called" << std::endl;
#endif
  }
  inline PartialTriang::PartialTriang(PartialTriang&& pt) : 
    pt_complex_type(std::move(pt)), 
    _no(pt._no),
    _rank(pt._rank),
    _admtableptr(pt._admtableptr),
    _inctableptr(pt._inctableptr),
    _voltableptr(pt._voltableptr),
    _admissibles(std::move(pt._admissibles)),
    _freeintfacets(std::move(pt._freeintfacets)),
    _freeconffacets(std::move(pt._freeconffacets)),
    _covered_volume(std::move(pt._covered_volume)) {
#ifdef CONSTRUCTOR_DEBUG
    std::cerr << "PartialTriang::PartialTriang(PartialTriang&& pt): move constructur called" << std::endl;
#endif
  }
  inline PartialTriang::PartialTriang(const PartialTriang& pt, const Simplex& new_simp) : 
    pt_complex_type(pt), 
    _no(pt._no),
    _rank(pt._rank),
    _admtableptr(pt._admtableptr),
    _inctableptr(pt._inctableptr),
    _voltableptr(pt._voltableptr),
    _admissibles(pt._admissibles),
    _freeintfacets(pt._freeintfacets),
    _freeconffacets(pt._freeconffacets),
    _covered_volume(pt._covered_volume) {
    _add_simplex(new_simp);
  }

  inline PartialTriang::PartialTriang(const PartialTriang& pt, 
				      const Simplex& new_simp, 
				      const SimplicialComplex& forbidden) : 
    pt_complex_type(pt), 
    _no(pt._no),
    _rank(pt._rank),
    _inctableptr(pt._inctableptr),
    _admtableptr(pt._admtableptr),
    _voltableptr(pt._voltableptr),
    _admissibles(pt._admissibles),
    _freeintfacets(pt._freeintfacets),
    _freeconffacets(pt._freeconffacets),
    _covered_volume(pt._covered_volume) {
    _add_simplex(new_simp, forbidden);
  }

  // destructor:
  inline PartialTriang::~PartialTriang() {}

  // assignment:
  inline PartialTriang& PartialTriang::operator=(const PartialTriang& pt) {
#ifdef CONSTRUCTOR_DEBUG
    std::cerr << "PartialTriang& PartialTriang::operator=(const PartialTriang& pt): copy assignment operator called" << std::endl;
#endif
    if (this == &pt) {
      return *this;
    }
    pt_complex_type::operator=(pt);
    _no = pt._no;
    _rank = pt._rank;
    _inctableptr       = pt._inctableptr;
    _admtableptr       = pt._admtableptr;
    _voltableptr       = pt._voltableptr;
    _admissibles       = pt._admissibles;
    _freeintfacets     = pt._freeintfacets;
    _freeconffacets    = pt._freeconffacets;
    _covered_volume    = pt._covered_volume;
    return *this;
  }

  inline PartialTriang& PartialTriang::operator=(PartialTriang&& pt) {
#ifdef CONSTRUCTOR_DEBUG
    std::cerr << "PartialTriang& PartialTriang::operator=(PartialTriang&& pt): move assignment operator called" << std::endl;
#endif
    if (this == &pt) {
      return *this;
    }
    pt_complex_type::operator=(std::move(pt));
    _no = pt._no;
    _rank = pt._rank;
    _inctableptr       = pt._inctableptr;
    _admtableptr       = pt._admtableptr;
    _voltableptr       = pt._voltableptr;
    _admissibles       = std::move(pt._admissibles);
    _freeintfacets     = std::move(pt._freeintfacets);
    _freeconffacets    = std::move(pt._freeconffacets);
    _covered_volume    = std::move(pt._covered_volume);
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
    if (_freeintfacets.empty()) {
      return true;
    }
    for (SimplicialComplex::iterator iter = _admissibles.begin(); 
	 iter != _admissibles.end(); 
	 ++iter) {
      PartialTriang new_partialtriang(*this, *iter);
      if (new_partialtriang.complete()) {
	*this = std::move(new_partialtriang);
	return true;
      }
    }
    return false;
  }

  // internal algorithms:
  inline void PartialTriang::_add_simplex(const Simplex& simp) {
    // *this += simp;
    this->insert(simp);
    _update_admissibles(simp);
    _update_freefacets(simp);
    _update_volume(simp);
  }
  inline void PartialTriang::_add_simplex(const Simplex& simp, const SimplicialComplex& forbidden) {
    // *this += simp;
    this->insert(simp);
    _update_admissibles(simp, forbidden);
    _update_freefacets(simp);
    _update_volume(simp);
  }
  inline void PartialTriang::_update_admissibles(const Simplex& simp) {
#ifndef STL_CONTAINERS
    _admissibles *= _admtableptr->find(simp)->data();
#else
    _admissibles *= _admtableptr->find(simp)->second;
#endif
  }
  inline void PartialTriang::_update_admissibles(const Simplex& simp, 
						 const SimplicialComplex& forbidden) {
#ifdef DEBUG
    std::cerr << "adm before: " << _admissibles << '\n';
    std::cerr << "adm of new simp: " << _admtableptr->find(simp)->second << '\n';
    std::cerr << "forbidden: " << forbidden << std::endl;
#endif
#ifndef STL_CONTAINERS
    _admissibles *= _admtableptr->find(simp)->data();
#else
    _admissibles *= _admtableptr->find(simp)->second;
#endif
    _admissibles -= forbidden;
#ifdef DEBUG
    std::cerr << "adm after: " << _admissibles << '\n';
#endif
  }
  inline void PartialTriang::_update_freefacets(const Simplex& simp) {
#ifndef STL_CONTAINERS
    _freeintfacets ^= _inctableptr->intfacets().find(simp)->data();
    _freeconffacets -= _inctableptr->conffacets().find(simp)->data();
#else
    _freeintfacets ^= _inctableptr->intfacets().find(simp)->second;
    // _freeconffacets -= _inctableptr->conffacets().find(simp)->second;
    const facets_data& covered_conffacets(_inctableptr->conffacets().find(simp)->second);
    for (facets_data::const_iterator conffacets_iter = covered_conffacets.begin();
	 conffacets_iter != covered_conffacets.end();
	 ++conffacets_iter) {
      _freeconffacets.erase(*conffacets_iter);
    }
#endif
  }

  inline void PartialTriang::_update_volume(const Simplex& simp) {
    if (_voltableptr && !_voltableptr->empty()) {
      _covered_volume += _voltableptr->find(simp)->second;
    }
  }

}; // namespace topcom

#endif

// eof PartialTriang.hh
