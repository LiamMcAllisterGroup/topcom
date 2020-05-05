#include <iostream>

#include "CommandlineOptions.hh"

#include "Symmetry.hh"
#include "Chirotope.hh"

int main (const int argc, const char** argv) {
  CommandlineOptions::init(argc, argv);
  if (CommandlineOptions::verbose()) {
    std::cerr << std::endl;
    std::cerr << "------------------------------------------------\n";
    std::cerr << "computing the chirotope of a point configuration\n";
    std::cerr << "------------------------------------------------\n";
    std::cerr << std::endl;
  }
  PointConfiguration points;
  if (std::cin >> points) {
    if ((points.coldim() < 2) || (points.rowdim() < 2)) {
      if (CommandlineOptions::verbose()) {
	std::cerr << "no of points and rank must be at least two." << std::endl;
      }
      return 1;
    }
    if (points.rowdim() > points.coldim()) {
      if (CommandlineOptions::verbose()) {
	std::cerr << "rank must not be larger than no of points." << std::endl;
      }
      return 1;
    }
    Chirotope chiro(points, true);
    chiro.print_string(std::cout);
    SymmetryGroup symmetry_generators(points.coldim());
    if (symmetry_generators.read_generators(std::cin)) {
      std::cout << symmetry_generators << std::endl;
    }
    else {
      if (CommandlineOptions::verbose()) {
	std::cerr << "no valid symmetry generators found." << std::endl;
      }
    }
    return 0;
  }
  else {
    if (CommandlineOptions::verbose()) {
      std::cerr << argv[0] << ": error while reading point configuration." << std::endl;
    }
    return 1;
  }
}
