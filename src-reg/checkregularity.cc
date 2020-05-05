#include "CommandlineOptions.hh"
#include "PointConfiguration.hh"
#include "Symmetry.hh"
#include "Chirotope.hh"
#include "PlacingTriang.hh"
#include "RegularityCheck.hh"


int main(const int argc, const char** argv) {
  CommandlineOptions::init(argc, argv);
  PointConfiguration points;
  if (!points.read(std::cin)) {
    if (CommandlineOptions::verbose()) {
      std::cerr << "error while reading point configuration" << std::endl;
    }
    return 1;
  }
  SymmetryGroup symmetries(points.no());
  symmetries.read(std::cin);
  Chirotope chiro(points, false);
  SimplicialComplex triang;
  size_type count(0);
  size_type count_nonregs(0);
  while (triang.read(std::cin)) {
    ++count;
    RegularityCheck regcheck(points, chiro, triang);
    if (!regcheck.is_regular()) {
      ++count_nonregs;
      std::cout << triang << std::endl << "is non-regular." << std::endl;
    }
    std::cerr << "Checked " << count << " triangulations, " 
	      << count_nonregs << " non-regular so far." << std::endl;
  }
  return 0;
}
