#include <iostream>

#include "CommandlineOptions.hh"

#include "Chirotope.hh"
#include "Symmetry.hh"

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
    std::cerr << "           computing the dual of a chirotope \n";
    std::cerr << "------------------------------------------------------------\n";
    std::cerr << std::endl;
  }
  Chirotope chirotope;
  if (chirotope.read_string(std::cin)) {
    chirotope.print_dualstring(std::cout);
    SymmetryGroup symmetries(chirotope.no());
    if (symmetries.read_generators(std::cin)) {
      symmetries.write_generators(std::cout);
      std::cout << std::endl;
    }
    else {
      if (CommandlineOptions::verbose()) {
	std::cerr << "no valid symmetry generators found." << std::endl;
      }
    }
    return 0;
  }
  else {
    return 1;
  }
}

// eof chiro2dual.cc
