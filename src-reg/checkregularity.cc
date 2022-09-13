#include "CommandlineOptions.hh"
#include "PointConfiguration.hh"
#include "Symmetry.hh"
#include "Chirotope.hh"
#include "Incidences.hh"
#include "PlacingTriang.hh"
#include "RegularityCheck.hh"

int main(const int argc, const char** argv) {

  using namespace topcom;
  
  CommandlineOptions::init(argc, argv);
  RegularityCheck::init();
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
  if (CommandlineOptions::verbose()) {
    std::cerr << "computing cocircuits ..." << std::endl;
  }
  Cocircuits cocircuits(chiro);
  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  if (CommandlineOptions::verbose()) {
    std::cerr << "computing facets ..." << std::endl;
  }
  Facets facets(cocircuits);
  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  if (CommandlineOptions::verbose()) {
    std::cerr << "computing incidences ..." << std::endl;
  }
  Incidences incidences(chiro, facets);
  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  while (triang.read(std::cin)) {
    ++count;
    RegularityCheck regcheck(points, chiro, incidences, triang);
    if (!regcheck.is_regular()) {
      ++count_nonregs;
      std::cout << triang << std::endl << "is non-regular." << std::endl;
    }
    std::cerr << "checked " << count << " triangulations, " 
	      << count_nonregs << " non-regular so far." << std::endl;
  }
  RegularityCheck::term();
  return 0;
}


// eof checkregularity.cc
