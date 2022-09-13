////////////////////////////////////////////////////////////////////////////////
// 
// Vertices.cc
//
//    produced: 14 Jul 2021 jr
// last change: 14 Jul 2021 jr
// 
////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <ctype.h>
#include <string.h>

#include "Vertices.hh"

namespace topcom {

  Vertices::Vertices(const Circuits& circuits) : 
    vertices_data(0, circuits.no()), _no(circuits.no()), _rank(circuits.rank()) {

    // remove from points all points that are singleton elements of some circuit:
    for (Circuits::const_iterator iter = circuits.begin();
	 iter != circuits.end();
	 ++iter) {
      if (iter->second.first.card() == 1) {
	*this -= iter->second.first;
      }
      else if (iter->second.second.card() == 1) {
	*this -= iter->second.second;
      }
    }
  }

  std::ostream& Vertices::write(std::ostream& ost) const {
    ost << _no << ',' << _rank << ':' << std::endl;
    ost << (const vertices_data&)*this;
    ost << std::endl;
    return ost;
  }

  std::istream& Vertices::read(std::istream& ist) {
    char c;

    clear();
    if (!(ist >> std::ws >> _no)) {
#ifdef READ_DEBUG
      std::cerr << "Vertices::read_string(std::istream&): "
		<< "number of points not found." << std::endl;
#endif
      ist.clear(std::ios::failbit);
      return ist;
    }
    if (!(ist >> std::ws >> c)) {
#ifdef READ_DEBUG
      std::cerr << "Vertices::read_string(std::istream&): "
		<< "separator not found." << std::endl;
#endif
      ist.clear(std::ios::failbit);
      return ist;
    }
    if (!(ist >> std::ws >> _rank)) {
#ifdef READ_DEBUG
      std::cerr << "Vertices::read_string(std::istream&): "
		<< "rank not found." << std::endl;
#endif
      ist.clear(std::ios::failbit);
      return ist;
    }
    if (!(ist >> std::ws >> c)) {
#ifdef READ_DEBUG
      std::cerr << "Vertices::read_string(std::istream&): "
		<< "separator not found." << std::endl;
#endif
      ist.clear(std::ios::failbit);
      return ist;
    }
    ist >> (vertices_data&)*this;
    return ist;
  }

}; // namespace topcom
  
// eof Facets.cc
