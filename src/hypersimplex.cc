#include <iostream>
#include <ctype.h>
#include <assert.h>

#include "Array.hh"
#include "HashMap.hh"
#include "Field.hh"
#include "Symmetry.hh"
#include "PointConfiguration.hh"
#include "Permutation.hh"
#include "IntegerSet.hh"

typedef HashMap<Vector, size_type> column_index_map_type;


// the following function enumerates all sum-element subsets of dim numbers
// via DFS and adds the characteristic vector to a point configuration:

void add_ones_dfs(const size_type dim, 
		  const size_type sum,
		  const size_type start, 
		  const size_type stop,
		  Vector& new_column,
		  PointConfiguration& points,
		  column_index_map_type& col_index) {
  if (sum == 0) {

    // we build a map that stores the column index for each column vector:
    col_index.insert(new_column, points.no());

    // add the new column to the configuration:
    points.append(new_column);
  }
  else {
    for (int i = start; i < stop; ++i) {
      new_column[i] = ONE;
      add_ones_dfs(dim, sum - 1, i + 1, stop, new_column, points, col_index);
      new_column[i] = ZERO;
    }
  }
}


int main(const int argc, const char** argv) {
  if (argc < 3) {
    std::cerr << "usage: " << argv[0] << " <coordinate sum> <ambient dimension>" << std::endl;
    return 1;
  }
  size_type sum(atol(argv[1]));
  size_type dim(atol(argv[2]));
  PointConfiguration hypersimplex;
  column_index_map_type col_index;

  // we start with an empty vector:
  Vector new_column(dim, ZERO);

  add_ones_dfs(dim, sum, 0, dim, new_column, hypersimplex, col_index);

  std::cout << hypersimplex << std::endl;

  // to find the symmetries of S_n, we let each pi in S_n act on each column
  // and find out what the column index of the result is:

  symmetry_data symmetry_generators;

  for (size_type i = 0; i < dim; ++i) {
    for (size_type j = i + 1; j < dim; ++j) {
      
      // this symmetry is the result of swapping coordinates i and j:
      Symmetry new_symmetry(hypersimplex.no());

      for (size_type preimage_idx = 0; 
	   preimage_idx < hypersimplex.maxindex();
	     ++preimage_idx) {

	// get the column vector to compute the image vector:
	Vector image_vec  = hypersimplex[preimage_idx];

	// swap position i and j:
	Field tmp         = image_vec[i];
	image_vec[i]      = image_vec[j];
	image_vec[j]      = tmp;

	// find the index of the image:
	assert(col_index.member(image_vec));
	const size_type image_idx = col_index[image_vec];

	// set the corresponding symmetry entry:
	new_symmetry[preimage_idx] = image_idx;
	
      }
      symmetry_generators.insert(new_symmetry);
    }
  }
  
  // print symmetry generators:
  std::cout << symmetry_generators << std::endl;
  
  return 0;
}
