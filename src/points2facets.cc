#include <iostream>

#include "CommandlineOptions.hh"

#include "Chirotope.hh"
#include "PlacingTriang.hh"
#include "Cocircuits.hh"
#include "Facets.hh"

int main (const int argc, const char** argv) {
  CommandlineOptions::init(argc, argv);
  if (CommandlineOptions::verbose()) {
    std::cerr << std::endl;
    std::cerr << "---------------------------------------------\n";
    std::cerr << "computing the facets of a point configuration\n";
    std::cerr << "---------------------------------------------\n";
    std::cerr << std::endl;
  }
  PointConfiguration points;
  if (points.read(std::cin)) {
    Chirotope chiro(points, false);
    //    Cocircuits cocircuits(chiro, true);

    // PlacingTriang method:
    PlacingTriang triang(chiro);
    Facets(chiro, triang.boundary_triang()).write(std::cout);
    return 0;
  }
  else {
    return 1;
  }
}
