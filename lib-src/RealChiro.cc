////////////////////////////////////////////////////////////////////////////////
// 
// RealChiro.cc
//
//    produced: 13 Mar 1998 jr
// last change: 30 Jun 2000 jr
// 
////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <ctype.h>
#include <string.h>
#include <assert.h>

#include "RealChiro.hh"

namespace topcom {

  const char    RealChiro::start_char = '[';
  const char    RealChiro::end_char   = ']';
  const char    RealChiro::delim_char = ',';
  const char*   RealChiro::map_chars = "->";

  RealChiro::RealChiro(const PointConfiguration& points, bool preprocess) : 
    chirotope_data(), _no(points.coldim()), _rank(points.rowdim()) {
    for (parameter_type i = 0; i < _no - _rank + 1; ++i) {
      _recursive_chiro(points[i], points, basis_type(i), i + 1, 1, false);
    }
    _has_dets = true;
    if (CommandlineOptions::verbose()) {
      std::cerr << std::endl;
      std::cerr << size() << " signs in total." << std::endl;
    }
  }

  const int RealChiro::operator()(const basis_type&  prebasis,
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
      basis_perm.push_back(lex_extension_perm[i]);
      int perm_sign = basis_perm.sign();
      return perm_sign * basis_sign;
    }
    return 0;
  }
  // functions:
  void RealChiro::erase_random() {
#ifdef STL_CHIROTOPE
    chirotope_data::iterator iter = this->begin();
    chirotope_data::erase(iter);
#else
    chirotope_data::erase_random();
#endif
  }

  const basis_type RealChiro::find_non_deg_basis() const {
    basis_type result;
    if (_no == 0) {
      return result;
    }
    if (_rank == 0) {
      return result;
    }
    Permutation perm(_no, _rank);
    result = basis_type(perm);
    while ( ((*this)(result) == 0) && (perm.lexnext()) ) {
      result = basis_type(perm);
    }
    assert((*this)(result) != 0);
    return result;
  }

  RealChiro RealChiro::dual() const {
    static long count;
    const basis_type groundset(0, _no);
    RealChiro result;
    result._no = _no;
    result._rank = _no - _rank;
    for (const_iterator iter = begin(); iter != end(); ++iter) {
#ifdef STL_CHIROTOPE
      const basis_type basis = iter->first;
      const int basis_sign = iter->second.first;
#else
      const basis_type basis = iter->key();
      const int basis_sign = iter->data().first;
#endif
      const basis_type dualbasis(groundset - basis);
      Permutation perm(dualbasis);
      perm.push_back(Permutation(basis));
      int perm_sign = perm.sign(result._rank);
      result[dualbasis] = std::pair<int, Field>(perm_sign * basis_sign, Field(perm_sign * basis_sign));
#ifdef COMPUTATIONS_DEBUG
      if (perm.sign() != perm_sign) {
	std::cerr << "RealChiro RealChiro::dual() const: "
		  << "perm_sign error." << std::endl;
      }
#endif
#ifdef SUPER_VERBOSE
      std::cerr << perm_sign * (*this)(basis) << ',';
#endif
      if (CommandlineOptions::verbose() && (++count % CommandlineOptions::report_frequency() == 0)) {
#ifdef SUPER_VERBOSE
	std::cerr << std::endl;
#endif
	std::cerr << count << " signs computed so far." << std::endl;
      }
    }
    if (CommandlineOptions::verbose()) {
      std::cerr << size() << " signs in total." << std::endl;
    }
    return result;
  }

  // stream output/input:
  std::ostream& RealChiro::print_string(std::ostream& ost) const {
    size_type count(0);
    ost << _no << ',' << _rank << ':' << std::endl;
    Permutation perm(_no, _rank);
    do {
      int chiro_on_perm = (*this)(basis_type(perm));
#ifdef SUPER_VERBOSE
      std::cerr << "chiro(" << basis_type(perm) << ") = " << chiro_on_perm << std::endl;
#endif
      ost << int2sign(chiro_on_perm);
      if (++count % 100 == 0) {
	ost << '\n';
      }
    } while (perm.lexnext());
    ost << std::endl;
#ifdef WATCH_MAXCHAINLEN
    std::cerr << "max chain length of hash table: " << maxchainlen() << std::endl;
#endif
    return ost;
  }

  std::ostream& RealChiro::print_dualstring(std::ostream& ost) const {
    static long count;
    const basis_type groundset(0, _no);
    ost << _no << ',' << _no - _rank << ':' << std::endl;
    Permutation dualperm(_no, _no -_rank);
    do {
      const Permutation primalperm(groundset - basis_type(dualperm));
      const basis_type basis(primalperm);
      Permutation perm(dualperm);
      perm.push_back(primalperm);
      const int perm_sign = perm.sign(_no - _rank);
      const int chiro_on_perm = perm_sign * (*this)(basis);
      ost << int2sign(chiro_on_perm);
      if (++count % 100 == 0) {
	ost << '\n';
      }
#ifdef COMPUTATIONS_DEBUG
      if (perm.sign() != perm_sign) {
	std::cerr << "RealChiro RealChiro::print_dualstring() const: "
		  << "perm_sign error." << std::endl;
      }
#endif
#ifdef SUPER_VERBOSE
      std::cerr << perm_sign * (*this)(basis) << ',';
#endif
      if (count % CommandlineOptions::report_frequency() == 0) {
	if (CommandlineOptions::verbose()) {
#ifdef SUPER_VERBOSE
	  std::cerr << std::endl;
#endif
	  std::cerr << count << " signs computed so far." << std::endl;
	}
      }
    } while (dualperm.lexnext());
    ost << std::endl;
    if (CommandlineOptions::verbose()) {
      std::cerr << count << " signs in total." << std::endl;
    }
    return ost;
  }

  std::istream& RealChiro::read_string(std::istream& ist) {
    char c;

    clear();
    if (!(ist >> std::ws >> _no)) {
#ifdef READ_DEBUG
      std::cerr << "RealChiro::read_string(std::istream&): "
		<< "number of points not found." << std::endl;
#endif
      ist.clear(std::ios::failbit);
      return ist;
    }
    if (!(ist >> std::ws >> c)) {
#ifdef READ_DEBUG
      std::cerr << "RealChiro::read_string(std::istream&): "
		<< "separator not found." << std::endl;
#endif
      ist.clear(std::ios::failbit);
      return ist;
    }
    if (!(ist >> std::ws >> _rank)) {
#ifdef READ_DEBUG
      std::cerr << "RealChiro::read_string(std::istream&): "
		<< "rank not found." << std::endl;
#endif
      ist.clear(std::ios::failbit);
      return ist;
    }
    if (!(ist >> std::ws >> c)) {
#ifdef READ_DEBUG
      std::cerr << "RealChiro::read_string(std::istream&): "
		<< "separator not found." << std::endl;
#endif
      ist.clear(std::ios::failbit);
      return ist;
    }
#ifdef MAXNOIS64
    if (_no > block_len) {
      std::cerr << "RealChiro::read(std::istream& ist): " << std::endl
		<< "Code was compiled for at most "
		<< block_len << " elements, " << std::endl
		<< "but the input chirotope has "
		<< no() << " elements." << std::endl;
      exit(1);
    }
#endif

    if (_rank > _no) {
#ifdef READ_DEBUG
      std::cerr << "Circuits::read_string(std::istream&): "
		<< "rank must not be larger than number of points." << std::endl;
#endif
      ist.clear(std::ios::failbit);
      return ist;    
    }
    Permutation perm(_no, _rank);
    do {
      if (ist >> c) {
	const int new_val = sign2int(c);
	(*this)[basis_type(perm)] = std::pair<int, Field>(new_val, Field(new_val));
      }
      else {
#ifdef READ_DEBUG
	std::cerr << "Circuits::read_string(std::istream&): "
		  << "not enough signs." << std::endl;
#endif
	ist.clear(std::ios::failbit);
	return ist;
      }
    } while (perm.lexnext());
    return ist;
  }

  // internal algorithms:
  void RealChiro::_recursive_chiro(const StairCaseMatrix&    current,
				   const PointConfiguration& points,
				   const basis_type&         basis,
				   const parameter_type      start,
				   const parameter_type      step,
				   const bool                already_dependent) {
    static long count;
    if (already_dependent) {
      for (parameter_type i = start; i < _no - _rank + step + 1; ++i) {
	basis_type newbasis(basis);
	newbasis += i;
	if (step + 1 == _rank) {
	  const Field next_det = FieldConstants::ZERO;
	  (*this)[newbasis] = std::pair<int, Field>(0, FieldConstants::ZERO);
	  if (CommandlineOptions::verbose()) {
#ifdef MEGA_VERBOSE
	    std::cerr << "inserting sign for basis: " << basis << std::endl;
#endif
	    if (++count % CommandlineOptions::report_frequency() == 0) {
#ifdef SUPER_VERBOSE
	      std::cerr << std::endl;
#endif
	      std::cerr << count << " signs computed so far." << std::endl;
	    }
	  }
	}
	else {
	  _recursive_chiro(current, points, newbasis, i + 1, step + 1, true);
	}
      }
    }
    else {
      for (parameter_type i = start; i < _no - _rank + step + 1; ++i) {
	StairCaseMatrix next = current;
	next.augment(points[i]);
	if (CommandlineOptions::debug()) {
	  Matrix raw(current);
	  raw.augment(points[i]);
	  std::cerr << "matrix with new column:" << std::endl;
	  raw.pretty_print(std::cerr);
	  std::cerr << std::endl;
	  std::cerr << "new staircase matrix:" << std::endl;
	  next.pretty_print(std::cerr);
	  std::cerr << std::endl;
	}
	basis_type newbasis(basis);
	newbasis += i;
	if (step + 1 == _rank) {
#ifdef SUPER_VERBOSE
	  std::cerr << next.det() << ',';
#endif
	  if (CommandlineOptions::debug()) {
	    std::cerr << "determinant at leaf: " << next.det() << std::endl;
	  }
	  const Field next_det = next.det();
	  (*this)[newbasis] = std::pair<int, Field>(sign(next_det), next_det);
	  if (CommandlineOptions::verbose()) {
#ifdef MEGA_VERBOSE
	    std::cerr << "inserting sign for basis: " << basis << std::endl;
#endif
	    if (++count % CommandlineOptions::report_frequency() == 0) {
#ifdef SUPER_VERBOSE
	      std::cerr << std::endl;
#endif
	      std::cerr << count << " signs computed so far." << std::endl;
	    }
	  }
	}
	else {
	  if (next.has_no_zerocol()) {
	    _recursive_chiro(next, points, newbasis, i + 1, step + 1, false);
	  }
	  else {	  
#ifdef SUPER_VERBOSE
	    std::cerr << "premature dependence found in corank " << _rank - step << std::endl;
#endif
	    if (CommandlineOptions::debug()) {
	      std::cerr << "deadend because of dependency." << std::endl;
	    }
	    _recursive_chiro(next, points, newbasis, i + 1, step + 1, true);
	  }
	}
      }
    }
  }

  // stream input/output:
  std::istream& RealChiro::read(std::istream& ist) {
    char dash;
    char arrow;
    char opening_bracket;
    char closing_bracket;
    char comma;

    basis_type basis_reader;
    int        int_reader;
    Field      field_reader;

    char c;

    clear();
    if (!(ist >> std::ws >> _no >> std::ws >> c >> std::ws >> _rank >> std::ws >> c >> std::ws >> c)) {
      if (CommandlineOptions::debug()) {
	std::cerr << "std::istream& RealChiro::read(std::istream&): "
		  << "`no, rank:' not found." << std::endl;
      }
      ist.clear(std::ios::failbit);
      return ist;
    };
    if (c == start_char) {
      while (ist >> std::ws >> c) {
	if (c == end_char) {
	  break;
	}
	if (c == delim_char) {
	  continue;
	}
	ist.putback(c);
	if (!(ist >> std::ws >> basis_reader)) {
	  if (CommandlineOptions::debug()) {
	    std::cerr << "std::istream& operator>>(std::istream&, chirotope_data&): "
		      << "key not found." << std::endl;
	  }
	  ist.clear(std::ios::failbit);
	  return ist;
	}
	if (!(ist >> std::ws >> dash >> arrow)) {
	  if (CommandlineOptions::debug()) {
	    std::cerr << "std::istream& operator>>(std::istream&, chirotope_data&): "
		      << "`" << map_chars << "' not found." << std::endl;
	  }
	  ist.clear(std::ios::failbit);
	  return ist;
	}
	if (!(ist >> std::ws >> opening_bracket >> int_reader)) {
	  if (CommandlineOptions::debug()) {
	    std::cerr << "std::istream& operator>>(std::istream&, chirotope_data&): "
		      << "data not found." << std::endl;
	  }
	  ist.clear(std::ios::failbit);
	  return ist;
	}
	if (!(ist >> std::ws >> comma >> field_reader >> closing_bracket)) {
	  if (CommandlineOptions::debug()) {
	    std::cerr << "std::istream& operator>>(std::istream&, chirotope_data&): "
		      << "data not found." << std::endl;
	  }
	  ist.clear(std::ios::failbit);
	  return ist;
	}
#ifndef STL_CHIROTOPE
	insert(basis_reader, Pair<int, Field>(int_reader, field_reader));
#else
	insert(std::pair<basis_type, std::pair<int, Field> >(basis_reader, std::pair<int, Field>(int_reader, field_reader)));
#endif
      }
    }
    else {
      if (CommandlineOptions::debug()) {
	std::cerr << "std::istream& operator>>(std::istream&, chirotope_data&): "
		  << "missing `" << start_char << "'." << std::endl;
      }
      ist.clear(std::ios::failbit);
      return ist;
    }
    ist.clear(std::ios::goodbit);
    return ist;
  }

  std::ostream& RealChiro::write(std::ostream& ost) const {
    ost << no() << ',' << rank() << ':' << start_char;
    if (!empty()) {
      chirotope_data::const_iterator iter = begin();
#ifndef STL_CHIROTOPE
      ost << iter->key() << map_chars << iter->data();
#else
      ost << iter->first << map_chars << iter->second;
#endif
      while (++iter != end()) {
#ifndef STL_CHIROTOPE
	ost << delim_char << iter->key() << map_chars << iter->data();
#else
	ost << delim_char << iter->first << map_chars << iter->second;
#endif
      }
    }
    ost << end_char;
    return ost;
  }

}; // namespace topcom

// eof RealChiro.cc
