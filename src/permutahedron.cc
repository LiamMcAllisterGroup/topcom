#include <iostream>
#include <ctype.h>
#include <assert.h>
#include <unordered_map>

#include "Global.hh"
#include "Field.hh"
#include "Symmetry.hh"
#include "PointConfiguration.hh"
#include "Permutation.hh"
#include "IntegerSet.hh"

int main(const int argc, const char** argv) {
  using namespace topcom;

  if (argc < 2) {
    std::cerr << "usage: " << argv[0] << " <dimension>" << std::endl;
    return 1;
  }

  size_type no = atol(argv[1]);

  symmetry_collectordata rowperms;
  
  Permutation sorted(Permutation(no + 1, 1).complement());
  Vector seed(sorted);
  PointConfiguration Permutahedron;
  Permutahedron.augment(seed);

  // travers through all two cycles to generate the entire symmetric group on rows:
  Permutation perm(no, 2);
  do {
    rowperms.insert(Symmetry(perm, true));
  } while (perm.lexnext());

  SymmetryGroup rowsyms(no, rowperms, true);

  for (SymmetryGroup::const_iterator symiter = rowsyms.begin();
       symiter != rowsyms.end();
       ++symiter) {
    Permutahedron.augment(symiter->map(seed));
  }

  symmetry_collectordata rowperm_generators(rowsyms.rowperm_symmetry_generators(Permutahedron));

  SymmetryGroup symmetries(Permutahedron.no(), rowperm_generators);
  
  // print points:
  std::cout << Permutahedron << std::endl;
  
  // print symmetry generators:
  symmetries.write_generators(std::cout);
  std::cout << std::endl;
  return 0;
}

// eof hypersimplex.cc
