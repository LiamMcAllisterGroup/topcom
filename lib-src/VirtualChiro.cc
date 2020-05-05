////////////////////////////////////////////////////////////////////////////////
// 
// VirtualChiro.cc
//
//    produced: 19 Nov 1999 jr
// last change: 30 Jun 2000 jr
// 
////////////////////////////////////////////////////////////////////////////////

#include <assert.h>

#include "VirtualChiro.hh"
const int VirtualChiro::operator()(const basis_type&  prebasis,
				   const Permutation& lex_extension_perm) const {
  assert(lex_extension_perm.n() == no());
  assert(lex_extension_perm.k() <= no());
  Permutation basis_perm(no(), rank() - 1, prebasis);
  basis_type basis(prebasis);
  for (size_type i = 0; i < lex_extension_perm.k(); ++i) {
    if (basis.contains(lex_extension_perm[i])) {
      continue;
    }
    basis += lex_extension_perm[i];
    const int basis_sign((*this)(basis));
    if (basis_sign == 0) {
      basis -= lex_extension_perm[i];
      continue;
    }
    basis_perm.append(lex_extension_perm[i]);
    int perm_sign = basis_perm.sign();
    return perm_sign * basis_sign;
  }
  return 0;
}

const basis_type VirtualChiro::find_non_deg_basis() const {
  if (_complete) {
    return _chiro.find_non_deg_basis();
  }
  basis_type result;
  if (_pointsptr->no() == 0) {
    return result;
  }
  if (_pointsptr->rank() == 0) {
    return result;
  }
  if (_recursive_find_non_deg_basis((*_pointsptr)[0], basis_type(0), 1, 1, result)) {
    return result;
  }
  else {
    std::cerr << "VirtualChiro::find_non_deg_basis() const:";
    std::cerr << "no non-degenerate basis exists." << std::endl;
    exit(1);
  }
}

const bool VirtualChiro::_recursive_find_non_deg_basis(const StairCaseMatrix&    current,
						       const basis_type&         basis,
						       const parameter_type      start,
						       const parameter_type      step,
						       basis_type&               result) const {
  for (parameter_type i = start; i < _pointsptr->no() - _pointsptr->rank() + step + 1; ++i) {
    StairCaseMatrix next = current;
    next.augment((*_pointsptr)[i]);
    const basis_type newbasis(basis + i);
    if (CommandlineOptions::debug()) {
      std::cerr << "partial basis matrix:" << std::endl;
      next.pretty_print(std::cerr);
    }
    if (step + 1 == _pointsptr->rank()) {
      if (det(next) != 0) {
	result = newbasis;
	return true;
      }
    }
    else {
      if (next.has_full_rank()) {
#ifdef DEBUG
	std::cerr << "VirtualChiro::_recursive_find_non_deg_basis:";
	std::cerr << "step " << step << std::endl;
#endif
 	if (_recursive_find_non_deg_basis(next, newbasis, i + 1, step + 1, result)) {
	  return true;
	}
      }
    }
  }
  return false;
}

// eof VirtualChiro.cc
