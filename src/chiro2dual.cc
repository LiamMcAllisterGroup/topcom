#include <iostream>

#include "CommandlineOptions.hh"

#include "Chirotope.hh"

int main (const int argc, const char** argv) {
  CommandlineOptions::init(argc, argv);
  if (CommandlineOptions::verbose()) {
    std::cerr << std::endl;
    std::cerr << "----------------------------------\n";
    std::cerr << "computing the dual of a chirotope \n";
    std::cerr << "----------------------------------\n";
    std::cerr << std::endl;
  }
  Chirotope chirotope;
  if (chirotope.read_string(std::cin)) {
    chirotope.print_dualstring(std::cout);
    return 0;
  }
  else {
    return 1;
  }
}
