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
  PointConfiguration cross;
  cross.push_back(Vector(1, FieldConstants::MINUSONE));
  cross.push_back(Vector(1, FieldConstants::ONE));
  PointConfiguration interval(cross);
  for (size_type i = 1; i < d; ++i) {
    cross.direct_sum(interval);
  }
  cross.homogenize();
  std::cout << cross << std::endl;
  // symmetries to be added.
  size_type n(2*d);
  std::vector<Permutation> pa;
  if (d > 0) {
    for (size_type i = 0; i < d; ++i) {     
      Permutation planereflect(n, n);
      size_type tmp(planereflect[2 * i]);
      planereflect[2 * i] = planereflect[(2 * i + 1) % n];
      planereflect[(2 * i + 1) % n] = tmp;
      pa.push_back(planereflect);
    }
    Permutation pointreflect(n, n);
    for (size_type i = 0; i < d; ++i) {
      size_type tmp(pointreflect[2 * i]);
      pointreflect[2 * i] = pointreflect[(2*i + 1) % n];
      pointreflect[(2 * i + 1) % n] = tmp;
    }
    pa.push_back(pointreflect);
  }
  std::cout << pa << std::endl;
  return 0;
}

// eof cross.cc
