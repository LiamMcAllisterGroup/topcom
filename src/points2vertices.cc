#include <iostream>

#include "CommandlineOptions.hh"

#include "Chirotope.hh"
#include "PlacingTriang.hh"
#include "Cocircuits.hh"
#include "Vertices.hh"

int main (const int argc, const char** argv) {
  using namespace topcom;

  CommandlineOptions::init(argc, argv);
    if (CommandlineOptions::verbose()) {
      std::cerr << std::endl;
      std::cerr << "------------------------------------------------------------\n";
      std::cerr << "------------------ " << PACKAGE << " VERSION " << VERSION << " -------------------\n";
      std::cerr << "Triangulations of Point Configurations and Oriented Matroids\n";
      std::cerr << "--------------------- by Joerg Rambau ----------------------\n";
      std::cerr << "------------------------------------------------------------\n";
      std::cerr << std::endl;
    }
    if (CommandlineOptions::verbose()) {
      std::cerr << std::endl;
      std::cerr << "------------------------------------------------------------\n";
      std::cerr << " computing circuits of a point configuration up to symmetry \n";
      std::cerr << "------------------------------------------------------------\n";
      std::cerr << std::endl;
#ifdef STL_CONTAINERS
      std::cerr << "        -- using STL containers for hash tables --" << std::endl;
#endif
#ifdef STL_SYMMETRIES
      std::cerr << "        -- using STL containers for symmetries  --" << std::endl;
#endif
      std::cerr << std::endl;
    }
  PointConfiguration points;
  if (points.read(std::cin)) {
    Chirotope chiro(points, false);

    // circuits method:
    Vertices(Circuits(chiro)).write(std::cout);
    return 0;
  }
  else {
    return 1;
  }
}

// eof points2facets.cc
