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
#include <queue>
#include <mutex>

#ifdef USE_SPARSEINTSET
#include "SparseIntegerSet.hh"
#else
#include "LabelSet.hh"
#endif
#include "HashMap.hh"

#include "CommandlineOptions.hh"

#include "StairCaseMatrix.hh"
#include "PointConfiguration.hh"
#include "Permutation.hh"
#include "RealChiro.hh"

namespace topcom {

  class VirtualChiro {
    const   PointConfiguration*    _pointsptr;
    mutable RealChiro              _chiro;
    mutable bool                   _complete;
    mutable std::mutex             _mutex;
    mutable std::queue<basis_type> _known_bases;
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
    inline Field det(const basis_type& basis) const;
    inline Field det(const Permutation& basisperm) const;
    
    // operators:
    inline const bool operator==(const VirtualChiro& chiro) const;
    inline const bool operator!=(const VirtualChiro& chiro) const;
    inline const int operator()(const basis_type& basis) const;
    inline const int operator()(const Permutation& perm) const;

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

  inline const int VirtualChiro::operator()(const basis_type& basis) const {
      if (_complete) {
	return _chiro(basis);
      }
      bool basis_known = false;
#ifndef STL_CHIROTOPE
      {
	std::lock_guard<std::mutex> lock(_mutex);
	basis_known = _chiro.member(basis);
	if (basis_known) {
	  return _chiro(basis);
	}
      }
#else
      {
	std::lock_guard<std::mutex> lock(_mutex);
	chirotope_data::const_iterator finder = _chiro.find(basis);
	if (finder != _chiro.end()) {
	  return finder->second.first;
	}
      }
#endif
	
#ifdef INDEX_CHECK
      assert(_pointsptr != 0);
      std::cerr << "computing new chirotope value ..." << std::endl;
#endif
      StairCaseMatrix basis_matrix;
      for (basis_type::const_iterator iter = basis.begin();
	   iter != basis.end();
	   ++iter) {
	basis_matrix.augment((*_pointsptr)[*iter]);
      }
      const Field result_det = basis_matrix.det();
      const int result_sign(sign(result_det));
      if (CommandlineOptions::memopt()) {
	if (CommandlineOptions::chirocache() > 0) {
	  std::lock_guard<std::mutex> lock(_mutex);
	  if (CommandlineOptions::chirocache() < _chiro.size() + 1) {
	    _chiro.erase(_known_bases.front()); // FIFO caching
	    _known_bases.pop();
	  }
	  _chiro[basis] = std::pair<int, Field>(result_sign, result_det);
	  _known_bases.push(basis);
	}
      }
      else {
	std::lock_guard<std::mutex> lock(_mutex);
	_chiro[basis] = std::pair<int, Field>(result_sign, result_det);
      }
#ifdef INDEX_CHECK
      std::cerr << "... done: " << result << std::endl;
#endif
      return result_sign;
    }

  inline const int VirtualChiro::operator()(const Permutation& perm) const { 
    assert(perm.n() == no());
    assert(perm.k() == rank());
    const basis_type basis(perm);    
    return perm.sign() * (*this)(basis);
  }

  inline Field VirtualChiro::det(const basis_type& basis) const {
    if (_complete && _chiro.has_dets()) {
      return _chiro.det(basis);
    }
    bool basis_known = false;
#ifndef STL_CHIROTOPE
    {
      std::lock_guard<std::mutex> lock(_mutex);
      basis_known = _chiro.member(basis);
      if (basis_known) {
	return _chiro.det(basis);
      }
    }
#else
    {
      std::lock_guard<std::mutex> lock(_mutex);
      chirotope_data::const_iterator finder = _chiro.find(basis);
      if (finder != _chiro.end()) {
	return finder->second.second;
      }
    }
#endif
    
#ifdef INDEX_CHECK
    assert(_pointsptr != 0);
    std::cerr << "computing new chirotope value ..." << std::endl;
#endif
    StairCaseMatrix basis_matrix;
    for (basis_type::const_iterator iter = basis.begin();
	 iter != basis.end();
	 ++iter) {
      basis_matrix.augment((*_pointsptr)[*iter]);
    }
    const Field result_det = basis_matrix.det();
    const int result_sign(sign(result_det));
    if (CommandlineOptions::memopt()) {
      if (CommandlineOptions::chirocache() > 0) {
	std::lock_guard<std::mutex> lock(_mutex);
	if (CommandlineOptions::chirocache() < _chiro.size() + 1) {
	  _chiro.erase(_known_bases.front()); // FIFO caching
	  _known_bases.pop();
	}
	_chiro[basis] = std::pair<int, Field>(result_sign, result_det);
	_known_bases.push(basis);
      }
    }
    else {
      std::lock_guard<std::mutex> lock(_mutex);
      _chiro[basis] = std::pair<int, Field>(result_sign, result_det);
    }
#ifdef INDEX_CHECK
    std::cerr << "... done: " << result << std::endl;
#endif
    return result_det;
  }
  
  inline Field VirtualChiro::det(const Permutation& basisperm) const {
    if (_complete && _chiro.has_dets()) {
      return _chiro.det(basisperm);
    }
    int permsign = basisperm.sign();
    if (permsign > 0) {
      return det(basis_type(basisperm));
    }
    else {
      return -det(basis_type(basisperm));
    }
  }
  
  inline const bool VirtualChiro::operator==(const VirtualChiro& chiro) const {
    return (*(this->_pointsptr) == *(chiro._pointsptr));
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
    _chiro(points.no(), points.rank(), true),
    _complete(false) {
    if (immediate && !CommandlineOptions::memopt()) {
      if (CommandlineOptions::verbose()) {
	std::cerr << "preprocessing chirotope with " << _binomial(_chiro.no(), _chiro.rank()) << " signs ..." << std::endl;
      }
      _chiro = RealChiro(points);
      if (CommandlineOptions::verbose()) {
	std::cerr << "... done." << std::endl;
      }
      _complete = true;
    }
  }

  inline VirtualChiro::~VirtualChiro() {
#ifdef CONSTRUCTOR_DEBUG
    std::cerr << "VirtualChiro::~VirtualChiro(): destructor called" << std::endl;
#endif
  }

};  // namespace topcom


#endif

// eof VirtualChiro.hh
