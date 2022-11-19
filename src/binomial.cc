#include <iostream>
#include <assert.h>

#include "Integer.h"


int main(const int argc, const char** argv) {
  using namespace topcom;

  if (argc < 3) {
    std::cerr << "usage: " << argv[0] << "<ambient cardinality> <subset cardinality>" << std::endl;
    return 1;
  }
  Integer n(argv[1]);
  Integer k(argv[2]);

  
  Integer kk(k);
  if (n - k < k) {
    kk = n - k;
  }
  if (k == 0) {
    std::cout << 1 << std::endl;
  }
  if (k == 1) {
    std::cout << n << std::endl;
  }
  if (kk < 1) {
    std::cout << 1 << std::endl;
  }
  Integer result(1);
  for (Integer i = 0; i < kk; ++i) {
    result *= n - i;
      result /= i + 1;
  }
  std::cout << result << std::endl;
  
  return 0;
}

// eof hypersimplex.cc
