#include <iostream>
#include <ctype.h>
#include <vector>

#include "Field.hh"
#include "Symmetry.hh"
#include "PointConfiguration.hh"
#include "Permutation.hh"
#include "IntegerSet.hh"

namespace topcom {

Permutation shift(Permutation& p) {
  Permutation result(p.n() + p.k(), p.k());
  for (size_type i = 0; i < p.k(); ++i) {
    result[i] = p[i] + p.k();
  }
  return result;
}

Permutation interleave(const Permutation& p1, const Permutation& p2) {
#ifdef INDEX_CHECK
  assert(p1.k() == p2.k());
#endif
  const size_type k(p1.k() + p2.k());
  const size_type n = p1.n() < p2.n() ? p2.n() : p1.n();
  Permutation result(n,k);
  for (size_type i = 0; i < p1.k(); ++i) {
    result[2 * i] = p1[i];
    result[2 * i + 1] = p2[i];
  }
  return result;
}

};

int main(const int argc, const char** argv) {
  using namespace topcom;

  if (argc < 2) {
    std::cerr << "usage: " << argv[0] << " d" << std::endl;
    return 1;
  }
  size_type d(atol(argv[1]));
  PointConfiguration cube;
  if (d < 0) {
    std::cerr << "usage: " << argv[0] << " d (where d >= 0)" << std::endl;
    return 1;
  }
  if (d == 0) {
    cube.push_back(Vector(1, FieldConstants::ZERO));
  }
  else {
    cube.push_back(Vector(1, FieldConstants::ZERO));
    cube.push_back(Vector(1, FieldConstants::ONE));
  }
  for (size_type i = 1; i < d; ++i) {
    cube.prism();
  }
  cube.homogenize();
  std::cout << cube << std::endl;
  // symmetries:
  std::vector<Permutation> pa;
  pa.push_back(Permutation(1,1));
  for (size_type i = 0; i < d; ++i) {
    std::vector<Permutation> new_pa;
    Permutation old_p;
    Permutation p1;
    Permutation p2;
    for (size_type i = 0; i < pa.size(); ++i) {
      old_p = pa[i];
      p1 = old_p;
      p2 = shift(old_p);
      // prisms of old symmetries:
      new_pa.push_back(p2.push_back(p1));
    }
    // new skew symmetry plane:
    p1 = old_p;
    p2 = shift(old_p);
    Permutation p3(interleave(p1, p2));
    new_pa.push_back(p3);
    pa = new_pa;
  }
  std::cout << pa << std::endl;
  return 0;
}

// eof cube.cc
