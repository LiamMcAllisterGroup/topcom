#include <iostream>
#include <ctype.h>

#include "Array.hh"
#include "Field.hh"
#include "Symmetry.hh"
#include "PointConfiguration.hh"
#include "Permutation.hh"
#include "IntegerSet.hh"

int main(const int argc, const char** argv) {
  if (argc < 2) {
    std::cerr << "usage: " << argv[0] << " d" << std::endl;
    return 1;
  }
  size_type d(atol(argv[1]));
  PointConfiguration cross;
  cross.append(Vector(1, MINUSONE));
  cross.append(Vector(1, ONE));
  PointConfiguration interval(cross);
  for (size_type i = 1; i < d; ++i) {
    cross.direct_sum(interval);
  }
  cross.homogenize();
  std::cout << cross << std::endl;
  // symmetries to be added.
  return 0;
}
