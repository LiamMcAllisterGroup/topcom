#include <iostream>
#include <ctype.h>

#include "PointConfiguration.hh"

int main(const int argc, const char** argv) {
  using namespace topcom;

  if (argc < 3) {
    std::cerr << "usage: " << argv[0] << " n d" << std::endl;
    return 1;
  }
  size_type n = atoi(argv[1]);
  size_type d = atoi(argv[2]);
  if (n <= d) {
    std::cerr << "n > d required" << std::endl;
    return 1;
  }
  PointConfiguration cyclic;
  for (size_type i = 0; i < n; ++i) {
    Vector column(d + 1);
    Field entry((long)(i));
    for (size_type j = 0; j < d; ++j) {
      column[j] = entry;
      entry *= i;
    }
    column[d] = FieldConstants::ONE;
    cyclic.push_back(column);
  }
  std::cout << cyclic << std::endl;
  std::cout << "[";
  if (d % 2 == 0) {
    std::cout << "[" << n - 1 ;
    for (size_type i = 0; i < n - 1; ++i) {
      std::cout << ',' << i;
    }
    std::cout << "],";
  }
  std::cout << "[";
  for (size_type i = 0; i < n - 1; ++i) {
    std::cout << n - i - 1 << ',';
  }
  std::cout << "0]]" << std::endl;
  return 0;
}

// eof cyclic.cc
