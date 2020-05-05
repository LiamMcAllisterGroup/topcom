#////////////////////////////////////////////////////////////////////////////////
// 
// Cocircuits.cc
//
//    produced: 12/09/2006 jr
// last change: 12/09/2006 jr
// 
////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <ctype.h>
#include <string.h>
#include <assert.h>

#include "Cocircuits.hh"

// Cocircuit:

Cocircuit::Cocircuit(const Chirotope& chiro, const IntegerSet& coplanar_set) {
  assert(coplanar_set.card() == chiro.rank() - 1);
  basis_type basis(coplanar_set);
  int pm(0);
  bool found_nonzero(false);
  IntegerSet universe(0, chiro.no());
  for (IntegerSet::iterator iter = universe.begin();
       iter != universe.end();
       ++iter) {
    if (coplanar_set.contains(*iter)) {
      pm = -pm;
    }
    else {
      basis += *iter;
      if (CommandlineOptions::debug()) {
	std::cerr << "chiro(" << basis << ") = " << std::endl;
      }
      int chiro_on_basis = chiro(basis);
      if (CommandlineOptions::debug()) {
	std::cerr << chiro_on_basis << std::endl;
      }
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
      basis -= *iter;
    }
  }
}

// Cocircuits:
Cocircuits::Cocircuits(const Chirotope& chiro, const bool only_positive) : 
  cocircuits_data(), _no(chiro.no()), _rank(chiro.rank()) {
  size_type count(0);
  // if (_no == _rank) {
  //   return;
  // }
  Permutation coplanar_perm(_no, _rank - 1);
  do {
    IntegerSet coplanar_set(coplanar_perm);
    if (CommandlineOptions::debug()) {
      std::cerr << "computing ";
      if (only_positive) {
	std::cerr << " positive ";
      }
      std::cerr << " sign vectors from spanning " 
	       << _rank - 1 << "-subset " 
	       << coplanar_set
	       << " ..."
	       << std::endl;
    }
    Cocircuit cocircuit(chiro, coplanar_set);
    if (CommandlineOptions::debug()) {
      std::cerr << "... done." << std::endl;
      std::cerr << "result: " << cocircuit << std::endl;
    }
    if ((only_positive) && !cocircuit.first.is_empty() && !cocircuit.second.is_empty()) {
      continue;
    }
    if (!cocircuit.first.is_empty() || !cocircuit.second.is_empty()) {
      insert(cocircuit.first + cocircuit.second, cocircuit);
      if (CommandlineOptions::verbose() && (++count % 10000 == 0)) {
	std::cerr << load();
	if (only_positive) {
	  std::cerr << " positive ";
	}
	std::cerr << " cocircuits computed so far." << std::endl;
      }
    }
  } while (coplanar_perm.lexnext());
  if (CommandlineOptions::verbose()) {
    std::cerr << load() << " cocircuits in total." << std::endl;
  }
}

// stream output/input:
std::ostream& Cocircuits::print_string(std::ostream& ost) const {
  ost << _no << ',' << _rank << ':' << std::endl;
  ost << '{' << std::endl;
  for (const_iterator iter = begin(); iter != end(); ++iter) {
    ost << '[' << iter->dataptr()->first << ',' << iter->dataptr()->second << ']' << '\n';
  }
  ost << '}' << std::endl;
  return ost;
}
std::istream& Cocircuits::read_string(std::istream& ist) {
  char     c;
  Cocircuit  cocircuit;

  clear();
  if (!(ist >> std::ws >> _no)) {
#ifdef READ_DEBUG
    std::cerr << "Cocircuits::read_string(std::istream&): "
	 << "number of points not found." << std::endl;
#endif
    ist.clear(std::ios::failbit);
    return ist;
  }
  if (!(ist >> std::ws >> c)) {
#ifdef READ_DEBUG
    std::cerr << "Cocircuits::read_string(std::istream&): "
	 << "separator not found." << std::endl;
#endif
    ist.clear(std::ios::failbit);
    return ist;
  }
  if (!(ist >> std::ws >> _rank)) {
#ifdef READ_DEBUG
    std::cerr << "Cocircuits::read_string(std::istream&): "
	 << "rank not found." << std::endl;
#endif
    ist.clear(std::ios::failbit);
    return ist;
  }
  if (!(ist >> std::ws >> c)) {
#ifdef READ_DEBUG
    std::cerr << "Cocircuits::read_string(std::istream&): "
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
      if (ist >> cocircuit) {
	insert(cocircuit.first + cocircuit.second, cocircuit);
      }
      else {
#ifdef READ_DEBUG
	std::cerr << "Cocircuits::read_string(std::istream&): "
	     << "not of appropriate type." << std::endl;
#endif
	ist.clear(std::ios::failbit);
	return ist;		
      }
    }
  }
  return ist;
}

// eof Cocircuits.cc
