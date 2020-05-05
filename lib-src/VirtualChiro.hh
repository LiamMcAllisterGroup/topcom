////////////////////////////////////////////////////////////////////////////////
// 
// VirtualChiro.hh 
//
//    produced: 19 Nov 1999 jr
// last change: 19 Nov 1999 jr
//
////////////////////////////////////////////////////////////////////////////////
#ifndef VIRTUALCHIRO_HH
#define VIRTUALCHIRO_HH

#include <assert.h>
#include <iostream>


#include "Array.hh"
#ifdef USE_SPARSEINTSET
#include "SparseIntegerSet.hh"
#else
#include "IntegerSet.hh"
#endif
#include "HashMap.hh"

#include "CommandlineOptions.hh"

#include "StairCaseMatrix.hh"
#include "PointConfiguration.hh"
#include "Permutation.hh"
#include "RealChiro.hh"

class VirtualChiro {
  const PointConfiguration* _pointsptr;
  mutable RealChiro         _chiro;
  mutable bool              _complete;
public:
  // constructors:
  inline VirtualChiro();
  inline VirtualChiro(const VirtualChiro&);
  inline VirtualChiro(const PointConfiguration&, const bool = true);
  // destructor:
  inline ~VirtualChiro();
  // assignment:
  inline VirtualChiro& operator=(const VirtualChiro& chiro) {
    if (this == &chiro) {
      return *this;
    }
    _pointsptr = chiro._pointsptr;
    _chiro     = chiro._chiro;
    _complete  = chiro._complete;
    return *this;
  }
  // complete chiro:
  inline void complete() const {
    if (!_complete) {
      _chiro = RealChiro(*_pointsptr);
      _complete = true;
    }
  }
  // accessors:
  inline parameter_type no()   const { return _chiro.no(); }
  inline parameter_type rank() const { return _chiro.rank(); };  
  // operators:
  inline const bool operator==(const VirtualChiro& chiro) const;
  inline const bool operator!=(const VirtualChiro& chiro) const;
  inline const int operator()(const basis_type& basis) const {
#ifndef STL_CHIROTOPE
    if (!(_chiro.member(basis))) {
#else
    if (_chiro.find(basis) == _chiro.end()) {
#endif
#ifdef INDEX_CHECK
      assert(_pointsptr != 0);
#endif
      StairCaseMatrix basis_matrix;
      for (basis_type::const_iterator iter = basis.begin();
	   iter != basis.end();
	   ++iter) {
	basis_matrix.augment((*_pointsptr)[*iter]);
      }
      const int result(sign(det(basis_matrix)));
      if (CommandlineOptions::memopt()) {
	if (CommandlineOptions::chirocache() > 0) {
	  if (CommandlineOptions::chirocache() < _chiro.load() + 1) {
	    _chiro.erase_random();
	  }
	  _chiro[basis] = result;
	}
      }
      else {
 	_chiro[basis] = result;
      }
      return result;
    }
    else {
      return _chiro(basis);
    }
  }
  inline const int operator()(const Permutation& perm) const { 
    assert(perm.n() == no());
    assert(perm.k() == rank());
    const basis_type basis(perm);    
    return perm.sign() * (*this)(basis);
  }

  // find the sign of prebasis and the lexicographic extension 
  // corresponding to the permutation perm (all signs positive):
  const int operator()(const basis_type&  prebasis,
		       const Permutation& perm) const;
  inline const int operator()(const Permutation& prebasis_perm,
			      const Permutation& perm) const {
    assert(prebasis_perm.n() == no());
    assert(prebasis_perm.k() == rank());
    const basis_type basis(prebasis_perm);
    return perm.sign() * (*this)(basis, perm);
  }
  inline const int operator[](const basis_type& basis) const { 
    return (*this)(basis); 
  }
  inline const int operator[](const Permutation& perm) const { 
    return (*this)(perm);
  }
  // functions:
  const basis_type find_non_deg_basis() const;
  inline VirtualChiro dual() const {
    VirtualChiro result;
    complete();
    result._chiro = _chiro.dual();
    result._complete = true;
    return result;
  };
  // stream output/input:
  inline std::ostream& print_string(std::ostream& ost) const {
    complete();
    return _chiro.print_string(ost);
  }
  inline std::ostream& print_dualstring(std::ostream& ost) const {
    complete();
    return _chiro.print_dualstring(ost);    
  }
  inline std::istream& read_string(std::istream& ist) {
    _complete = true;
    return _chiro.read_string(ist);
  }
  // stream output/input:
  inline friend std::ostream& operator<<(std::ostream& ost, const VirtualChiro& chiro) {
    chiro.complete();
    return (ost << chiro._chiro);
  }
  inline friend std::istream& operator>>(std::istream& ist, VirtualChiro& chiro) {
    chiro._complete = true;
    return (ist >> chiro._chiro);
  }
  const bool _recursive_find_non_deg_basis(const StairCaseMatrix&    current,
					   const basis_type&         basis,
					   const parameter_type      start,
					   const parameter_type      step,
					   basis_type&               result) const;
};

inline const bool VirtualChiro::operator==(const VirtualChiro& chiro) const {
  complete();
  chiro.complete();
  return _chiro == chiro._chiro;
}

inline const bool VirtualChiro::operator!=(const VirtualChiro& chiro) const {
  return !operator==(chiro);
}

inline VirtualChiro::VirtualChiro() : 
  _pointsptr(0), _chiro(), _complete(false) {}

inline VirtualChiro::VirtualChiro(const VirtualChiro& chiro) : 
  _pointsptr(chiro._pointsptr), 
  _chiro(chiro._chiro) {}

inline VirtualChiro::VirtualChiro(const PointConfiguration& points,
				  const bool                immediate) : 
  _pointsptr(&points), 
  _chiro(points.no(), points.rank()),
  _complete(immediate) {
  if (immediate) {
    _chiro = RealChiro(points);
  }
//   else {
//     _chiro = RealChiro(points.no(), points.rank());
//   }
}

inline VirtualChiro::~VirtualChiro() {}

#endif

// eof VirtualChiro.hh
