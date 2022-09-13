#include <iostream>
#include <ctype.h>

#include "Symmetry.hh"
#include "PointConfiguration.hh"

int main(const int argc, const char** argv) {
  using namespace topcom;

  if (argc < 2) {
    std::cerr << "usage: " << argv[0] << " n" << std::endl;
    return 1;
  }
  parameter_type n = atoi(argv[1]);
  if (n <= 0) {
    std::cerr << "n > 0 required" << std::endl;
    return 1;
  }

  // first compute the group S_n consisting of all Permutations:
  symmetry_collectordata S_data;
  Permutation perm(n, 2);
  do {
    Symmetry s(perm, true);
    S_data.insert(s);
  } while (perm.lexnext());
  SymmetryGroup S(n, S_data);

  std::pair<PointConfiguration, SymmetryGroup> B_S_center(S.permutation_polytope(true));
  std::cout << B_S_center.first << std::endl;
  B_S_center.second.write_generators(std::cout);
  std::cout << std::endl;

  return 0;
}

// eof B_S_center.cc
