#include <iostream>
#include <ctype.h>
#include <vector>

#include "Field.hh"
#include "Symmetry.hh"
#include "PointConfiguration.hh"
#include "Permutation.hh"
#include "IntegerSet.hh"

int main(const int argc, const char** argv) {
  using namespace topcom;

  if (argc < 3) {
    std::cerr << "usage: " << argv[0] << " m n" << std::endl;
    return 1;
  }
  size_type m(atol(argv[1]));
  size_type n(atol(argv[2]));
  PointConfiguration lattice;
  for (int i = 0; i < m; ++i) {
    for (int j = 0; j < n; ++j) {
      Vector column(3, FieldConstants::ONE);
      column[0] = i;
      column[1] = j;
      lattice.push_back(column);
    }
  }
  std::cout << lattice << std::endl;
  // symmetries:
  std::vector<Permutation> pa;
  Permutation reflect_vertically(parameter_type(m*n), parameter_type(m*n));
  for (int i = 0; i < m; ++i) {
    for (int j = 0; j < n; ++j) {
      reflect_vertically[i*n+j] = i*n+(n-j-1);
    }
  }
  pa.push_back(reflect_vertically);
  if (m != n) {
    Permutation reflect_horizontally(parameter_type(m*n), parameter_type(m*n));
    for (int i = 0; i < m; ++i) {
      for (int j = 0; j < n; ++j) {
	reflect_horizontally[i*n+j] = (m-i-1)*n+j;
      }
    }
    pa.push_back(reflect_horizontally);
  }
  else {
    Permutation reflect_diagonally(parameter_type(m*n), parameter_type(m*n));
    for (int i = 0; i < m; ++i) {
      for (int j = 0; j < n; ++j) {
	reflect_diagonally[i*n+j] = j*m+i;
      }
    }
    pa.push_back(reflect_diagonally);
  }
  std::cout << pa << std::endl;
  return 0;
}

// eof lattice.cc
