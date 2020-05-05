#include <iostream>

#include "CommandlineOptions.hh"

#include "Circuits.hh"

int main (const int argc, const char** argv) {
  CommandlineOptions::init(argc, argv);
  if (CommandlineOptions::verbose()) {
    std::cerr << std::endl;
    std::cerr << "-------------------------------------\n";
    std::cerr << "computing the circuits of a chirotope\n";
    std::cerr << "-------------------------------------\n";
    std::cerr << std::endl;
  }
  Chirotope chiro;
  if (chiro.read_string(std::cin)) {
    Circuits(chiro).print_string(std::cout);
    return 0;
  }
  else {
    return 1;
  }
}
