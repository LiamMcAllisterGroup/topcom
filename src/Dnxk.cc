#include <iostream>
#include <ctype.h>
#include <assert.h>
#include <unordered_map>

#include "Field.hh"
#include "Vector.hh"
#include "Symmetry.hh"
#include "PointConfiguration.hh"
#include "Permutation.hh"
#include "IntegerSet.hh"

int main(const int argc, const char** argv) {
  using namespace topcom;

  if (argc < 3) {
    std::cerr << "usage: " << argv[0] << " <dimension> <multiplicity>" << std::endl;
    return 1;
  }
  size_type dim(atol(argv[1]));
  size_type mul(atol(argv[2]));

  PointConfiguration Dnxk;
  const Permutation identity(dim, dim);
  for (parameter_type i = 0; i < mul; ++i) {
    Dnxk.augment(identity_matrix(dim));
  }
  
  std::cout << Dnxk << std::endl;

  symmetry_collectordata perms;

  for (parameter_type i = 0; i < dim - 1; ++i) {

    // add simplex symmetries in all copies:
    Permutation perm(mul * dim, mul * dim);
    for (parameter_type j = 0; j < mul; ++j) {
      perm[j * dim + i] = j * dim + i + 1;
      perm[j * dim + i + 1] = j * dim + i;
    }
    perms.insert(Symmetry(perm, false));
  }

  for (parameter_type j = 0; j < mul - 1; ++j) {

    // add a symmetry swapping one point among the simplex copies:
    Permutation perm(mul * dim, 2);
    perm[0] = j * dim;
    perm[1] = (j + 1) * dim;
    perms.insert(Symmetry(perm, true));
  }
  
  SymmetryGroup symmetries(Dnxk.no(), perms, false);
  
  // print symmetry generators:
  symmetries.write_generators(std::cout);
  std::cout << std::endl;
  return 0;
}

// eof Dnxk.cc
