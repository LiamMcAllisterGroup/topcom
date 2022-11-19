#include <iostream>

#include "CommandlineOptions.hh"

#include "PlacingTriang.hh"

int main (const int argc, const char** argv) {
  using namespace topcom;

  CommandlineOptions::init(argc, argv);
  if (CommandlineOptions::verbose()) {
    std::cerr << std::endl;
    std::cerr << "-----------------------------------------\n";
    std::cerr << "computing a placing triang of a chirotope\n";
    std::cerr << "-----------------------------------------\n";
    std::cerr << std::endl;
  }
  Chirotope chiro;
  if (chiro.read_string(std::cin)) {
    std::cout << PlacingTriang(chiro) << std::endl;
    return 0;
  }
  else {
    if (CommandlineOptions::verbose()) {
      std::cerr << "chiro2placingtriang: error while reading chirotope." << std::endl;
    }
    return 1;
  }
}

// eof chiro2placingtriang.cc
