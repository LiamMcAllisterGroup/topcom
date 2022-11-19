#include <iostream>

#include "CommandlineOptions.hh"

#include "Facets.hh"

int main (const int argc, const char** argv) {
  using namespace topcom;

  CommandlineOptions::init(argc, argv);
  if (CommandlineOptions::verbose()) {
    std::cerr << std::endl;
    std::cerr << "-------------------------------------\n";
    std::cerr << "computing the facets of cocircuits   \n";
    std::cerr << "-------------------------------------\n";
    std::cerr << std::endl;
  }
  Cocircuits cocircuits;
  if (cocircuits.read_string(std::cin)) {
    Facets(cocircuits).write(std::cout);
    return 0;
  }
  else {
    return 1;
  }
}

// eof cocircuits2facets.cc
