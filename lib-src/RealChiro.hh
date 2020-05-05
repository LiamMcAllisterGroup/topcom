////////////////////////////////////////////////////////////////////////////////
// 
// RealChiro.hh 
//
//    produced: 13/03/98 jr
// last change: 13/03/98 jr
//
////////////////////////////////////////////////////////////////////////////////
#ifndef REALCHIRO_HH
#define REALCHIRO_HH

#include <assert.h>
#include <iostream>


#include "Array.hh"
#ifdef USE_SPARSEINTSET
#include "SparseIntegerSet.hh"
#else
#include "IntegerSet.hh"
#endif
#include "HashMap.hh"

#ifdef STL_CHIROTOPE
#include <tr1/unordered_map>
#endif

#include "CommandlineOptions.hh"

#include "StairCaseMatrix.hh"
#include "PointConfiguration.hh"
#include "Permutation.hh"

#ifdef USE_SPARSEINTSET
typedef SparseIntegerSet                basis_type;
#else
typedef IntegerSet                      basis_type;
#endif
#ifdef STL_CHIROTOPE
typedef std::tr1::unordered_map<basis_type, int> chirotope_data;

inline std::ostream& operator<<(std::ostream& ost, const chirotope_data& cd) {
  for (chirotope_data::const_iterator iter = cd.begin();
       iter != cd.end();
       ++iter) {
    ost << iter->first << "->" << iter->second;
  }
  return ost;
}

inline std::istream& operator>>(std::istream& ist, chirotope_data& cd) {
  char dash;
  char arrow;

  basis_type basis_reader;
  int        int_reader;

  char c;

  cd.clear();
  ist >> std::ws >> c;
  if (c == '[') {
    while (ist >> std::ws >> c) {
      if (c == ']') {
	break;
      }
      if (c == ',') {
	continue;
      }
      ist.putback(c);
      if (!(ist >> std::ws >> basis_reader)) {
#ifdef READ_DEBUG
	std::cerr << "std::istream& operator>>(std::istream&, chirotope_data&): "
		  << "key not found." << std::endl;
#endif
	ist.clear(std::ios::failbit);
	return ist;
      }
      if (!(ist >> std::ws >> dash >> arrow)) {
#ifdef READ_DEBUG
	std::cerr << "std::istream& operator>>(std::istream&, chirotope_data&): "
		  << "`->' not found." << std::endl;
#endif
	ist.clear(std::ios::failbit);
	return ist;
      }
      if (!(ist >> std::ws >> int_reader)) {
#ifdef READ_DEBUG
	std::cerr << "std::istream& operator>>(std::istream&, chirotope_data&): "
		  << "data not found." << std::endl;
#endif
	ist.clear(std::ios::failbit);
	return ist;
      }
      cd[basis_reader] = int_reader;
    }
  }
  else {
#ifdef READ_DEBUG
    std::cerr << "std::istream& operator>>(std::istream&, chirotope_data&): "
	 << "missing `" << '[' << "'." << std::endl;
#endif
    ist.clear(std::ios::failbit);
    return ist;
  }
  ist.clear(std::ios::goodbit);
  return ist;
}

#else
typedef HashMap<basis_type, int>                 chirotope_data;
#endif

inline const char int2sign(const int i) {
  switch(i) {
  case  1:	return '+';
  case -1:	return '-';
  default:	return '0';
  }
}

inline const int sign2int(const char c) {
  switch(c) {
  case '+':	return 1;
  case '-':	return -1;
  default:	return 0;
  }
}

class RealChiro : public chirotope_data {
  parameter_type  _no;
  parameter_type  _rank;
public:
  // constructors:
  inline RealChiro();
  inline RealChiro(const RealChiro&);
  inline RealChiro(const parameter_type&, const parameter_type&);
  RealChiro(const PointConfiguration&, bool preprocess=true);
  //assignment:
  inline RealChiro& operator=(const RealChiro& chiro);
  // accessors:
  inline parameter_type no() const;
  inline parameter_type rank() const;  
  inline size_type load() const;
  // functions:
  void erase_random();
  const basis_type find_non_deg_basis() const;
  RealChiro dual() const;
  // operators:
  inline const int operator()(const basis_type&) const;
  const int operator()(const basis_type&  prebasis,
		       const Permutation& lex_extension_perm) const;
  inline const bool operator==(const RealChiro&) const;
  inline const bool operator!=(const RealChiro&) const;
  // stream output/input:
  std::ostream& print_string(std::ostream&) const;
  std::ostream& print_dualstring(std::ostream&) const;
  std::istream& read_string(std::istream&);
  inline friend std::ostream& operator<<(std::ostream&, const RealChiro& chiro);
  inline friend std::istream& operator>>(std::istream&, RealChiro& chiro);
private:
  // internal algorithms:
  void _recursive_chiro(const StairCaseMatrix&    current,
			const PointConfiguration& points,
			const basis_type&         basis,
			const parameter_type      start,
			const parameter_type      step,
			const bool                already_dependent);
  void _recursive_find_nd_basis(const StairCaseMatrix&    current,
				const PointConfiguration& points,
				const basis_type&         basis,
				const parameter_type      start,
				const parameter_type      step,
				const bool                already_dependent);
};

inline RealChiro::RealChiro() : chirotope_data(), _no(0), _rank(0) {
}
inline RealChiro::RealChiro(const RealChiro& chiro) : 
  chirotope_data(chiro), _no(chiro._no), _rank(chiro._rank) {
}
inline RealChiro::RealChiro(const parameter_type& no,
			    const parameter_type& rank) :
  chirotope_data(), _no(no), _rank(rank) {}

//assignment:
inline RealChiro& RealChiro::operator=(const RealChiro& chiro) {
  if (this == &chiro) {
    return *this;
  }
  chirotope_data::operator=(chiro);
  _no = chiro._no;
  _rank = chiro._rank;
  return *this;
}

// accessors:
inline parameter_type RealChiro::no()   const { return _no; }
inline parameter_type RealChiro::rank() const { return _rank; }
inline parameter_type RealChiro::load() const { 
#ifdef STL_CHIROTOPE
  return chirotope_data::size();
#else
  return chirotope_data::load();
#endif
}

// operators:
inline const int RealChiro::operator()(const basis_type& basis) const {
#ifdef INDEX_CHECK
  if (!(member(basis))) {
    std::cerr << *this << " does not contain " << basis << std::endl;
    exit(1);
  }
#endif
#ifndef STL_CHIROTOPE
  return *member(basis);
#else
  return find(basis)->second;
#endif
}

inline const bool RealChiro::operator==(const RealChiro& chiro) const {
#ifdef STL_CHIROTOPE
  return ((*this) == chiro);
#else
  return chirotope_data::operator==(chiro);
#endif
}

inline const bool RealChiro::operator!=(const RealChiro& chiro) const {
  return !operator==(chiro);
}

// stream output/input:
inline std::ostream& operator<<(std::ostream& ost, const RealChiro& chiro) {
  ost << chiro._no << ',' << chiro._rank << ':' << (chirotope_data&)(chiro);
  return ost;
}

inline std::istream& operator>>(std::istream& ist, RealChiro& chiro) {
  char c;

  ist >> std::ws >> chiro._no >> std::ws >> c >> std::ws >> chiro._rank >> std::ws >> c >> std::ws >> (chirotope_data&)(chiro);
  return ist;
}

#endif

// eof RealChiro.hh
