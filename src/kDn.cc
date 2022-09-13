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

namespace topcom {

typedef std::unordered_map<Vector, size_type, Hash<Vector> > column_index_map_type;

// the following function enumerates all sum-element subsets of dim numbers
// via DFS and adds the characteristic vector to a point configuration:

void add_coords_dfs(const size_type dim, 
		    const size_type sum,
		    const size_type start, 
		    const size_type stop,
		    Vector& new_column,
		    PointConfiguration& points,
		    column_index_map_type& col_index) {
  if (sum == 0) {

    // we build a map that stores the column index for each column vector:
    col_index[new_column] = points.no();

    // add the new column to the configuration:
    points.push_back(new_column);
  }
  else {
    for (int i = start; i < stop; ++i) {
      for (int coord = sum; coord > 0; --coord) {
	new_column[i] = coord;
	add_coords_dfs(dim, sum - coord, i + 1, stop, new_column, points, col_index);
      }
      new_column[i] = FieldConstants::ZERO;
    }
  }
}

};

int main(const int argc, const char** argv) {
  using namespace topcom;

  if (argc < 3) {
    std::cerr << "usage: " << argv[0] << " <dilation factor> <dimension>" << std::endl;
    return 1;
  }
  size_type sum(atol(argv[1]));
  size_type dim(atol(argv[2]));
  PointConfiguration kDn;
  column_index_map_type col_index;

  // we start with an empty vector:
  Vector new_column(dim + 1, FieldConstants::ZERO);

  add_coords_dfs(dim + 1, sum, 0, dim + 1, new_column, kDn, col_index);

  std::cout << kDn << std::endl;

  symmetry_collectordata rowperms;

  // // travers through all two cycles to generate the entire symmetric group on rows:
  // Permutation perm(kDn.rank(), 2);
  // do {
  //   rowperms.insert(Symmetry(perm, true));
  // } while (perm.lexnext());

  // travers through all adjacent transpositions to find generators of the symmetric group on rows:
  for (parameter_type row = 0; row < kDn.rank() - 1; ++row) {
    Permutation rowperm(kDn.rank(), 2);
    rowperm[0] = row;
    rowperm[1] = row + 1;
    rowperms.insert(Symmetry(rowperm, true));
  }
  symmetry_collectordata rowperm_generators(SymmetryGroup(kDn.no(), rowperms, false).rowperm_symmetry_generators(kDn));
  SymmetryGroup symmetries(kDn.no(), rowperm_generators, false);
  // print symmetry generators:
  symmetries.write_generators(std::cout);
  std::cout << std::endl;
  return 0;
}

// eof kDn.cc
