#include <iostream>
#include <ctype.h>
#include <assert.h>
#include <unordered_map>

#include "Field.hh"
#include "Symmetry.hh"
#include "PointConfiguration.hh"
#include "Permutation.hh"
#include "IntegerSet.hh"

namespace topcom {

typedef std::unordered_map<Vector, size_type, Hash<Vector> > column_index_map_type;

// the following function enumerates all sum-element subsets of dim numbers
// via DFS and adds the characteristic vector to a point configuration:

void add_ones_dfs(const size_type dim, 
		  const size_type sum,
		  const size_type start, 
		  const size_type stop,
		  Vector& new_column,
		  PointConfiguration& points) {
  if (sum == 0) {

    // add the new column to the configuration:
    points.push_back(new_column);
  }
  else {
    for (int i = start; i < stop; ++i) {
      new_column[i] = FieldConstants::ONE;
      add_ones_dfs(dim, sum - 1, i + 1, stop, new_column, points);
      new_column[i] = FieldConstants::ZERO;
    }
  }
}

};

int main(const int argc, const char** argv) {
  using namespace topcom;

  bool three_parameters = false;
  if (argc < 3) {
    std::cerr << "usage: " << argv[0] << "<ambient dimension> <coordinate sum>[ <coordinate sum2>]" << std::endl;
    return 1;
  }
  size_type dim(atol(argv[1]));
  size_type sum(atol(argv[2]));
  if ((sum <= 0) || (sum >= dim)) {
    std::cerr << argv[0] << ": coordinate sums must be strictly between 0 and dimension - exiting" << std::endl;
    exit(1);
  }
      
  size_type sum2;
  if (argc > 3) {
    three_parameters = true;
    sum2 = atol(argv[3]);
    if ((sum2 <= 0) || (sum2 >= dim)) {
      std::cerr << argv[0] << ": coordinate sums must be strictly between 0 and dimension - exiting" << std::endl;
      exit(1);
    }
  }
  PointConfiguration hypersimplex;
  column_index_map_type col_index;

  // we start with an empty vector:
  Vector new_column(dim, FieldConstants::ZERO);

  add_ones_dfs(dim, sum, 0, dim, new_column, hypersimplex);

  if (three_parameters) {
    Vector new_column(dim, FieldConstants::ZERO);
    add_ones_dfs(dim, sum2, 0, dim, new_column, hypersimplex);
    hypersimplex.homogenize();
  }

  std::cout << hypersimplex << std::endl;

  symmetry_collectordata rowperms;
  // travers through all two cycles to generate the entire symmetric group on rows:
  Permutation perm(hypersimplex.rank(), 2);
  do {
    rowperms.insert(Symmetry(perm, true));
  } while (perm.lexnext());
  symmetry_collectordata rowperm_generators(SymmetryGroup(hypersimplex.no(), rowperms, false).rowperm_symmetry_generators(hypersimplex));
  SymmetryGroup symmetries(hypersimplex.no(), rowperm_generators, false);
  // print symmetry generators:
  symmetries.write_generators(std::cout);
  std::cout << std::endl;
  return 0;
}

// eof hypersimplex.cc
