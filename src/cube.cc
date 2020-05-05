#include <iostream>
#include <ctype.h>

#include "Array.hh"
#include "Field.hh"
#include "Symmetry.hh"
#include "PointConfiguration.hh"
#include "Permutation.hh"
#include "IntegerSet.hh"

PointConfiguration& prism(PointConfiguration& p) {
  PointConfiguration new_cols(p);
  for (size_type i = 0; i < p.coldim(); ++i) {
    p[i].append(ZERO);
    new_cols[i].append(ONE);
  }
  p.append(new_cols);
  return p;
}

PointConfiguration& homogenize(PointConfiguration& p) {
  for (size_type i = 0; i < p.coldim(); ++i) {
    p[i].append(ONE);
  }
  return p;
}

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

int main(const int argc, const char** argv) {
  if (argc < 2) {
    std::cerr << "usage: " << argv[0] << " d" << std::endl;
    return 1;
  }
  size_type d(atol(argv[1]));
  PointConfiguration cube;
  cube.append(Vector(1, ZERO));
  cube.append(Vector(1, ONE));
  for (size_type i = 1; i < d; ++i) {
    //    prism(cube);
    cube.prism();
  }
  //  homogenize(cube);
  cube.homogenize();
  std::cout << cube << std::endl;
  // symmetries:
  Array<Permutation> pa;
  pa.append(Permutation(1,1));
  for (size_type i = 0; i < d; ++i) {
    Array<Permutation> new_pa;
    Permutation old_p;
    Permutation p1;
    Permutation p2;
    for (size_type i = 0; i < pa.maxindex(); ++i) {
      old_p = pa[i];
      p1 = old_p;
      p2 = shift(old_p);
      // prisms of old symmetries:
      new_pa.append(p2.append(p1));
    }
    // new skew symmetry plane:
    p1 = old_p;
    p2 = shift(old_p);
    Permutation p3(interleave(p1, p2));
    new_pa.append(p3);
    pa = new_pa;
  }
  std::cout << pa << std::endl;
  return 0;
}
