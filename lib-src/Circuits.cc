////////////////////////////////////////////////////////////////////////////////
// 
// Circuits.cc
//
//    produced: 13/03/98 jr
// last change: 13/03/98 jr
// 
////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <ctype.h>
#include <string.h>
#include <assert.h>

#include "Circuits.hh"

// Circuit:
SimplicialComplex Circuit::upper_facets() const {
  SimplicialComplex result;
  Simplex simp(first);
  simp += second;
  for (Simplex::iterator iter = second.begin(); iter != second.end(); ++iter) {
    simp -= *iter;
    result += simp;
    simp += *iter;
  }
  return result;
}

SimplicialComplex Circuit::lower_facets() const {
  SimplicialComplex result;
  Simplex simp(first);
  simp += second;
  for (Simplex::iterator iter = first.begin(); iter != first.end(); ++iter) {
    simp -= *iter;
    result += simp;
    simp += *iter;
  }
  return result;
}

Circuit::Circuit(const Chirotope& chiro, const IntegerSet& dependent_set) {
  assert(dependent_set.card() == chiro.rank() + 1);
  basis_type basis(dependent_set);
  int pm(0);
  bool found_nonzero(false);
  for (IntegerSet::iterator iter = dependent_set.begin();
       iter != dependent_set.end();
       ++iter) {
    basis -= *iter;
    int chiro_on_basis = chiro(basis);
#ifdef SUPER_VERBOSE
    std::cerr << "chiro(" << basis << ") = " << chiro_on_basis << std::endl;
#endif
    if (!found_nonzero && (chiro_on_basis != 0)) {
      found_nonzero = true;
      pm = chiro_on_basis;
      first += *iter;
    }
    else {
      if (chiro_on_basis * pm > 0) {
	first += *iter;
      }
      else if (chiro_on_basis * pm < 0) {
	second += *iter;
      }
    }
    pm = -pm;
    basis += *iter;
  }
}

// Circuits:
Circuits::Circuits(const Chirotope& chiro) : 
  circuits_data(), _no(chiro.no()), _rank(chiro.rank()) {
  size_type count(0);
  if (_no == _rank) {
    return;
  }
  Permutation dependent_perm(_no, _rank + 1);
  do {
    IntegerSet dependent_set(dependent_perm);
    Circuit circuit(chiro, dependent_set);
    if (!circuit.first.is_empty() || !circuit.second.is_empty()) {
      insert(circuit.first + circuit.second, circuit);
      if (CommandlineOptions::verbose() && (++count % 10000 == 0)) {
	std::cerr << load() << " circuits computed so far." << std::endl;
      }
    }
  } while (dependent_perm.lexnext());
  if (CommandlineOptions::verbose()) {
    std::cerr << load() << " circuits in total." << std::endl;
  }
}

// stream output/input:
std::ostream& Circuits::print_string(std::ostream& ost) const {
  ost << _no << ',' << _rank << ':' << std::endl;
  ost << '{' << std::endl;
  for (const_iterator iter = begin(); iter != end(); ++iter) {
    ost << '[' << iter->dataptr()->first << ',' << iter->dataptr()->second << ']' << '\n';
  }
  ost << '}' << std::endl;
  return ost;
}
std::istream& Circuits::read_string(std::istream& ist) {
  char     c;
  Circuit  circuit;

  clear();
  if (!(ist >> std::ws >> _no)) {
#ifdef READ_DEBUG
    std::cerr << "Circuits::read_string(std::istream&): "
	 << "number of points not found." << std::endl;
#endif
    ist.clear(std::ios::failbit);
    return ist;
  }
  if (!(ist >> std::ws >> c)) {
#ifdef READ_DEBUG
    std::cerr << "Circuits::read_string(std::istream&): "
	 << "separator not found." << std::endl;
#endif
    ist.clear(std::ios::failbit);
    return ist;
  }
  if (!(ist >> std::ws >> _rank)) {
#ifdef READ_DEBUG
    std::cerr << "Circuits::read_string(std::istream&): "
	 << "rank not found." << std::endl;
#endif
    ist.clear(std::ios::failbit);
    return ist;
  }
  if (!(ist >> std::ws >> c)) {
#ifdef READ_DEBUG
    std::cerr << "Circuits::read_string(std::istream&): "
	 << "separator not found." << std::endl;
#endif
    ist.clear(std::ios::failbit);
    return ist;
  }
  while (ist >> std::ws >> c) {
    if (c == '{') {
      continue;
    }
    else if (c == '}') {
      break;
    }
    else {
      ist.putback(c);
      if (ist >> circuit) {
	insert(circuit.first + circuit.second, circuit);
      }
      else {
#ifdef READ_DEBUG
	std::cerr << "Circuits::read_string(std::istream&): "
	     << "not of appropriate type." << std::endl;
#endif
	ist.clear(std::ios::failbit);
	return ist;		
      }
    }
  }
  return ist;
}

// eof Circuits.cc
