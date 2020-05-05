#include <iostream>

#include "CommandlineOptions.hh"

#include "FineTriang.hh"

int main (const int argc, const char** argv) {
  CommandlineOptions::init(argc, argv);
  if (CommandlineOptions::verbose()) {
    std::cerr << std::endl;
    std::cerr << "------------------------------------------------\n";
    std::cerr << "computing a fine triang of a point configuration\n";
    std::cerr << "------------------------------------------------\n";
    std::cerr << std::endl;
  }
  Chirotope chiro;
  if (chiro.read_string(std::cin)) {
    std::cout << FineTriang(chiro) << std::endl;
    return 0;
  }
  else {
#ifdef VERBOSE
    std::cerr << "chiro2fulltriang: error while reading chirotope." << std::endl;
#endif
    return 1;
  }
}
