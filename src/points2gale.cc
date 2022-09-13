#include <iostream>

#include "CommandlineOptions.hh"

#include "Symmetry.hh"
#include "PointConfiguration.hh"

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
    std::cerr << "------------------------------------------------------\n";
    std::cerr << "computing the Gale transform of a point configuration \n";
    std::cerr << "------------------------------------------------------\n";
    std::cerr << std::endl;
  }
  PointConfiguration points;
  if (points.read(std::cin)) {
    if (CommandlineOptions::debug()) {
      std::cerr << "read points " << points << std::endl;
    }
    SymmetryGroup symmetries(points.no());
    if (!CommandlineOptions::ignore_symmetries()) {
      if (symmetries.read_generators(std::cin)) {
	if (CommandlineOptions::verbose()) {
	  std::cerr << symmetries.generators().size() + 1 << " symmetry generators in total." << std::endl;
	  if (CommandlineOptions::debug()) {
	    std::cerr << "symmetry generators:" << std::endl;
	    symmetries.write_generators(std::cout);
	    std::cout << std::endl;
	  }
	}
      }
    }
    else {
      if (CommandlineOptions::verbose()) {
	std::cerr << "no valid symmetry generators found." << std::endl;
      }
    }
    points.gale().write(std::cout);
    std::cout << std::endl;
    symmetries.write_generators(std::cout);
    std::cout << std::endl;
    return 0;
  }
  else {
    return 1;
  }
}

// eof points2gale.cc
