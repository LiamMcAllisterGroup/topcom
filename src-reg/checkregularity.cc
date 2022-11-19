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

  if (CommandlineOptions::verbose()) {
    std::cerr << std::endl;
    std::cerr << "------------------------------------------------------------\n";
    std::cerr << "------------------ " << PACKAGE << " VERSION " << VERSION << " --------------------\n";
    std::cerr << "Triangulations of Point Configurations and Oriented Matroids\n";
    std::cerr << "--------------------- by Joerg Rambau ----------------------\n";
    std::cerr << "------------------------------------------------------------\n";
    std::cerr << std::endl;
#ifdef STL_CONTAINERS
    std::cerr << "       -- using STL containers for hash tables --" << std::endl;
#endif
#ifdef STL_SYMMETRIES
    std::cerr << "       -- using STL containers for symmetries  --" << std::endl;
#endif
#ifdef STL_FLIPS
    std::cerr << "       --    using STL containers for flips    --" << std::endl;
#endif
#ifdef STL_CHIROTOPE
    std::cerr << "       -- using STL containers for chirotopes  --" << std::endl;
#endif
    std::cerr << std::endl;
  }

  PointConfiguration points;
  if (!points.read(std::cin)) {
    if (CommandlineOptions::verbose()) {
      std::cerr << "error while reading point configuration" << std::endl;
    }
    return 1;
  }
  if (points.rank() < points.rowdim()) {
    std::cerr << "point configuration has " << points.rowdim() << " rows of rank " << points.rank() << std::endl;
    points.transform_to_full_rank();
    std::cerr << "resulting no of rows after transformation: " << points.rank() << std::endl;
    points.pretty_print(std::cerr);
  }
  if (CommandlineOptions::verbose()) {
    std::cerr << "read point configuration with " << points.no()
	      << " points in rank " << points.rank() << std::endl;
  }
  
  SymmetryGroup symmetries(points.no());
  symmetries.read(std::cin);
  Chirotope chiro(points, false);
  SimplicialComplex triang;
  size_type count(0);
  size_type count_nonregs(0);
  if (CommandlineOptions::memopt()) {
    if (CommandlineOptions::verbose()) {
      std::cerr << "no preprocessing of simplex table to save memory" << std::endl;
    }
  }
  else {
    if (CommandlineOptions::verbose()) {
      std::cerr << "preprocessing simplex table up to rank " << points.rank() << " ..." << std::endl;
    }
    SimplicialComplex::preprocess_index_table(points.no(), 0, points.rank(), chiro, true);
    if (CommandlineOptions::verbose()) {
      std::cerr << "... done." << std::endl;
    }
  }
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
  if (CommandlineOptions::verbose()) {
    std::cerr << "checking input triangulations for regularity ..." << std::endl;
  }
  while (triang.read(std::cin)) {
    ++count;
    RegularityCheck regcheck(points, chiro, incidences, triang);
    if (!regcheck.is_regular()) {
      ++count_nonregs;
      std::cout << "T[" << count << "] := " << triang << " is non-regular." << std::endl;
    }
    else {
      std::cout << "T[" << count << "] := " << triang << " is regular." << std::endl;
      if (CommandlineOptions::output_heights()) {
	std::cout << "h[" << count
		  << "] := " << regcheck.heights() << ";\n";
      }
    }
    std::cerr << "checked " << count << " triangulations, " 
	      << count_nonregs << " non-regular so far." << std::endl;
  }
  if (CommandlineOptions::verbose()) {
    std::cerr << "... no more triangulations found." << std::endl;
  }
  RegularityCheck::term();
  return 0;
}


// eof checkregularity.cc
