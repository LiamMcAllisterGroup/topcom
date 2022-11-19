////////////////////////////////////////////////////////////////////////////////
// 
// Volumes.cc
//
//    produced: 18/02/2020 jr
// last change: 18/02/2020 jr
// 
////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <ctype.h>
#include <string.h>

#include "PointConfiguration.hh"
#include "Permutation.hh"
#include "Volumes.hh"

namespace topcom {

  const char    Volumes::start_char = '[';
  const char    Volumes::end_char   = ']';
  const char    Volumes::delim_char = ',';
  const char*   Volumes::map_chars = "->";


  Volumes::Volumes(const PointConfiguration& points, const bool only_unimodular_simplices) : 
    volumes_data() {
    size_type count(0);
    const size_type no(points.no());
    const size_type rank(points.rank());
    Permutation perm(no, rank);
    do {
      Simplex simp(perm);
      Field simp_vol(points.volume(simp));
      if (simp_vol != FieldConstants::ZERO) {
	if (only_unimodular_simplices && (simp_vol != FieldConstants::ONE)) {
	  continue;
	}
	(*this)[simp] = simp_vol;
      }
    } while (perm.lexnext());
    if (CommandlineOptions::verbose()) {
      std::cerr << size() << " valid simplices." << std::endl;
    }
  }
  // stream input/output:
  std::istream& Volumes::read(std::istream& ist) {
    return (ist >> (volumes_data&)(*this));
  }

  std::ostream& Volumes::write(std::ostream& ost) const {
    return (ost << (volumes_data&)(*this));
  }

}; // namespace topcom

// eof Volumes.cc
