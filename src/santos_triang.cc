// This program contains the construction of
// * the Santos point configuration,
// * its symmteries,
// * the Santos triangulation of it without flips
// according to the paper
// Francisco Santos: 
// ``A Point Set whose Space of Triangulations is Disconnected''
// Journal of the American Mathematical Society, to appear.


#include <iostream>

#include "CommandlineOptions.hh"

#include "PointConfiguration.hh"
#include "Symmetry.hh"
#include "SimplicialComplex.hh"

PointConfiguration create_h() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing h ..." << std::endl;
  }
  PointConfiguration result(Matrix(4, 8, ZERO));
  result[0][0] = Field(4);
  result[0][1] = Field(-4);
  result[1][0] = Field(6);
  result[2][0] = Field(4);
  result[2][1] = Field(4);
  result[3][1] = Field(6);
  result[4][0] = Field(-4);
  result[4][1] = Field(4);
  result[5][0] = Field(-6);
  result[6][0] = Field(-4);
  result[6][1] = Field(-4);
  result[7][1] = Field(-6);
  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  return result;
}

PointConfiguration create_v() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing v ..." << std::endl;
  }
  PointConfiguration result(Matrix(4, 8, ZERO));
  result[0][2] = Field(6);
  result[1][2] = Field(4);
  result[1][3] = Field(4);
  result[2][3] = Field(6);
  result[3][2] = Field(-4);
  result[3][3] = Field(4);
  result[4][2] = Field(-6);
  result[5][2] = Field(-4);
  result[5][3] = Field(-4);
  result[6][3] = Field(-6);
  result[7][2] = Field(4);
  result[7][3] = Field(-4);
  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  return result;
}

PointConfiguration create_t() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing t ..." << std::endl;
  }
  PointConfiguration result(Matrix(4, 64, ZERO));
  PointConfiguration h(create_h());
  PointConfiguration v(create_v());
  for (size_type i = 0; i < 8; ++i) {
    for (size_type j = 0; j < 8; ++j) {
      if ((i + j) % 2 == 1) {
	result[8*i + j] = h[i];
	result[8*i + j].add(v[j]);
      }
    }
  }
  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  return result;
}

PointConfiguration create_s() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing s ..." << std::endl;
  }
  PointConfiguration result(Matrix(4, 64, ZERO));
  PointConfiguration t(create_t());
  for (size_type i = 0; i < 8; ++i) {
    for (size_type j = 0; j < 8; ++j) {
      if ((i + j) % 2 == 0) {
	result[8*i + j] = t[8*i + ((j + 1) % 8)];
	result[8*i + j].add(t[8*((i + 1) % 8) + j]);
	result[8*i + j].scale(Field(0.5));
      }
    }
  }
  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  return result;
}

PointConfiguration create_st() {
  PointConfiguration result(create_t());
  result.add(create_s());
  return result;
}

PointConfiguration create_A() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing four-dimensional point configuration A from v, h, s, t ..." 
	 << std::endl;
  }
  Matrix result(Vector(4, ZERO));
  result.augment(create_h());
  result.augment(create_v());
  result.augment(create_st());
  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  return PointConfiguration(result);
}

PointConfiguration create_B() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing two-dimensional point configuration B ..." 
	 << std::endl;
  }
  Matrix result(2, 4, ZERO);
  result[0][0] = ONE;
  result[0][1] = ONE;
  result[1][0] = Field(3);
  result[2][1] = Field(3);
  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  return PointConfiguration(result);
}

PointConfiguration create_C() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing six-dimensional point configuration C from A and B ..." 
	 << std::endl;
  }
  PointConfiguration A(create_A());
  PointConfiguration B(create_B());
  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  return A.product(B);
}

Symmetry create_g1() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing g1 ..." << std::endl;
  }
  Symmetry result(81);
  result[0] = 0;
  for (size_type i = 0; i < 8; ++i) {
    result[1 + i] = 1 + ((i + 2) % 8);
  }
  for (size_type i = 0; i < 8; ++i) {
    result[9 + i] = 9 + i;
  }
  for (size_type i = 0; i < 8; ++i) {
    for (size_type j = 0; j < 8; ++j) {
      result[17 + 8*i + j] = 17 + 8*((i + 2) % 8) + j;
    }
  }
  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  return result;
}

Symmetry create_g2() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing g2 ..." << std::endl;
  }
  Symmetry result(81);
  result[0] = 0;
  for (size_type i = 0; i < 8; ++i) {
    result[1 + i] = 1 + i;
  }
  for (size_type i = 0; i < 8; ++i) {
    result[9 + i] = 9 + ((i + 2) % 8);
  }
  for (size_type i = 0; i < 8; ++i) {
    for (size_type j = 0; j < 8; ++j) {
      result[17 + 8*i + j] = 17 + 8*i + ((j + 2) % 8);
    }
  }
  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  return result;
}

Symmetry create_g3() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing g3 ..." << std::endl;
  }
  Symmetry result(81);
  result[0] = 0;
  for (size_type i = 0; i < 8; ++i) {
    result[1 + i] = 1 + ((i + 1) % 8);
  }
  for (size_type i = 0; i < 8; ++i) {
    result[9 + i] = 9 + ((i + 1) % 8);
  }
  for (size_type i = 0; i < 8; ++i) {
    for (size_type j = 0; j < 8; ++j) {
      result[17 + 8*i + j] = 17 + 8*((i + 1) % 8) + ((j + 1) % 8);
    }
  }
  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  return result;
}

Symmetry create_g4() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing g4 ..." << std::endl;
  }
  Symmetry result(81);
  result[0] = 0;
  for (size_type i = 0; i < 8; ++i) {
    result[1 + i] = 9 + i;
  }
  for (size_type i = 0; i < 8; ++i) {
    result[9 + i] = 1 + i;
  }
  for (size_type i = 0; i < 8; ++i) {
    for (size_type j = 0; j < 8; ++j) {
      result[17 + 8*i + j] = 17 + 8*j + i;
    }
  }
  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  return result;
}

SymmetryGroup create_G() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing symmetry group G of A from g1, g2, g3, g4 ..." 
	 << std::endl;
  }
  symmetry_data result;
  result.insert(create_g1());
  result.insert(create_g2());
  result.insert(create_g3());
  result.insert(create_g4());
  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  return SymmetryGroup(81, result);
}

SymmetryGroup create_H() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing symmetry group H of B ..." 
	 << std::endl;
  }
  symmetry_data result;
  Symmetry h1(4);
  Symmetry h2(4);
  h2[0] = 0;
  h2[1] = 2;
  h2[2] = 3;
  h2[3] = 1;
  result.insert(h1);
  result.insert(h2);
  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  return SymmetryGroup(4, result);
}

Symmetry create_product(const Symmetry& s1, const Symmetry& s2) {
  Symmetry result(s1.n() * s2.n());
  ProductIndex product_index(s1.n(), s2.n());
  for (size_type i = 0; i < s1.n(); ++i) {
    for (size_type j = 0; j < s2.n(); ++j) {
      result[product_index(i, j)] = product_index(s1[i], s2[j]);
    }
  }
  return result;
}

SymmetryGroup create_product(const SymmetryGroup& G, const SymmetryGroup& H) {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing symmetry group GH of C from G and H ..." 
	 << std::endl;
  }
  symmetry_data result;
  for (SymmetryGroup::const_iterator iter1 = G.begin();
       iter1 != G.end();
       ++iter1) {
    const Symmetry& sym1 = *iter1;
    const Symmetry& sym2 = Symmetry(H.n());
    result.insert(create_product(sym1, sym2));
  }
  for (SymmetryGroup::const_iterator iter2 = H.begin();
       iter2 != H.end();
       ++iter2) {
    const Symmetry& sym1 = Symmetry(G.n());
    const Symmetry& sym2 = *iter2;
    result.insert(create_product(sym1, sym2));
  }
  for (SymmetryGroup::const_iterator iter1 = G.begin();
       iter1 != G.end();
       ++iter1) {
    for (SymmetryGroup::const_iterator iter2 = H.begin();
	 iter2 != H.end();
	 ++iter2) {
      const Symmetry& sym1 = *iter1;
      const Symmetry& sym2 = *iter2;
      result.insert(create_product(sym1, sym2));
    }
  }
  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  return SymmetryGroup(G.n()*H.n(), result);
}

SymmetryGroup create_GH() {
  return create_product(create_G(), create_H());
}

Permutation create_A_ij() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing Alpha_11 ..." << std::endl;
  }
  Permutation result(81, 5);
  result[0] = 0;
  result[1] = (17 + 8*0 + 0);
  result[2] = (9 + 2);
  result[3] = (17 + 8*1 + 2);
  result[4] = (17 + 8*1 + 1);
  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  return result;
}

Permutation create_B_ij() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing Beta_11 ..." << std::endl;
  }
  Permutation result(81, 5);
  result[0] = 0;
  result[1] = (17 + 8*0 + 0);
  result[2] = (17 + 8*2 + 1);
  result[3] = (9 + 2);
  result[4] = (17 + 8*1 + 1);
  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  return result;
}

Permutation create_C_ij() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing Gamma_11 ..." << std::endl;
  }
  Permutation result(81, 5);
  result[0] = 0;
  result[1] = (17 + 8*2 + 1);
  result[2] = (9 + 2);
  result[3] = (17 + 8*1 + 1);
  result[4] = (17 + 8*3 + 2);
  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  return result;
}

Permutation create_D_ij() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing Delta_11 ..." << std::endl;
  }
  Permutation result(81, 5);
  result[0] = 0;
  result[1] = (17 + 8*0 + 1);
  result[2] = (17 + 8*0 + 0);
  result[3] = (9 + 2);
  result[4] = (17 + 8*1 + 2);
  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  return result;
}

Permutation create_E_ij() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing Epsilon_11 ..." << std::endl;
  }
  Permutation result(81, 5);
  result[0] = 0;
  result[1] = (9 + 1);
  result[2] = (17 + 8*0 + 0);
  result[3] = (17 + 8*2 + 1);
  result[4] = (9 + 2);
  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  return result;
}

Permutation create_F_ij() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing Zeta_11 ..." << std::endl;
  }
  Permutation result(81, 5);
  result[0] = 0;
  result[1] = (9 + 1);
  result[2] = (17 + 8*0 + 1);
  result[3] = (17 + 8*0 + 0);
  result[4] = (9 + 2);
  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  return result;
}

Permutation create_G_ij() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing Eta_11 ..." << std::endl;
  }
  Permutation result(81, 5);
  result[0] = (9 + 1);
  result[1] = (17 + 8*0 + 1);
  result[2] = (17 + 8*0 + 0);
  result[3] = (17 + 8*2 + 1);
  result[4] = (9 + 2);
  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  return result;
}

Permutation create_H_ij() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing Theta_11 ..." << std::endl;
  }
  Permutation result(81, 5);
  result[0] = (17 + 8*0 + 1);
  result[1] = (17 + 8*0 + 0);
  result[2] = (17 + 8*2 + 1);
  result[3] = (9 + 2);
  result[4] = (17 + 8*1 + 1);
  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  return result;
}

Permutation create_I_ij() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing Iota_11 ..." << std::endl;
  }
  Permutation result(81, 5);
  result[0] = (17 + 8*0 + 1);
  result[1] = (17 + 8*0 + 0);
  result[2] = (9 + 2);
  result[3] = (17 + 8*1 + 2);
  result[4] = (17 + 8*1 + 1);
  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  return result;
}

HashSet<Permutation> insert_orbit(const Permutation& s, const SymmetryGroup& G, 
				  HashSet<Permutation>& result) {
  result.insert(s);
  for (SymmetryGroup::const_iterator iter = G.begin();
       iter != G.end();
       ++iter) {
#ifdef NO_STL
    const Symmetry& g(iter->key());
#else
    const Symmetry& g(*iter);
#endif
    result.insert(g(s));
  }
  return result;
}

HashSet<Permutation> create_T() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing triangulation T of A" << std::endl
	 << "from Alpha, Beta, Gamma, Delta, Epsilon, Zeta, Eta, Theta, Iota ..." 
	 << std::endl;
  }
  HashSet<Permutation> result;
  const SymmetryGroup G(create_G());
  insert_orbit(create_A_ij(), G, result);
  insert_orbit(create_B_ij(), G, result);
  insert_orbit(create_C_ij(), G, result);
  insert_orbit(create_D_ij(), G, result);
  insert_orbit(create_E_ij(), G, result);
  insert_orbit(create_F_ij(), G, result);
  insert_orbit(create_G_ij(), G, result);
  insert_orbit(create_H_ij(), G, result);
  insert_orbit(create_I_ij(), G, result);
  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  return result;
}

HashSet<Permutation> create_S() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing triangulation S of B ..." << std::endl;
  }
  HashSet<Permutation> result;
  Permutation s1(4, 3);
  s1[0] = 0;
  s1[1] = 1;
  s1[2] = 2;
  Permutation s2(4, 3);
  s2[0] = 0;
  s2[1] = 2;
  s2[2] = 3;
  Permutation s3(4, 3);
  s3[0] = 0;
  s3[1] = 3;
  s3[2] = 1;
  result.insert(s1);
  result.insert(s2);
  result.insert(s3);
  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  return result;
}

SimplicialComplex create_simplicial_complex(const HashSet<Permutation>& T) {
  SimplicialComplex result;
  for (HashSet<Permutation>::const_iterator iter = T.begin();
       iter != T.end();
       ++iter) {
    result += Simplex(*iter);
  }
  return result;
}

SimplicialComplex create_staircase_triang(const Permutation& p1, const Permutation& p2, 
					  const size_type start1 = 0, const size_type start2 = 0) {
  const ProductIndex product_index(p1.n(), p2.n());
  SimplicialComplex result;
  if (start1 == p1.k() - 1) {
    Simplex chain;
    for (size_type i = start2; i < p2.k(); ++i) {
      chain += product_index(p1[start1], p2[i]);
    }
    result += chain;
    return result;
  }
  if (start2 == p2.k() - 1) {
    Simplex chain;
    for (size_type i = start1; i < p1.k(); ++i) {
      chain += product_index(p1[i], p2[start2]);
    }
    result += chain;
    return result;
  }
  result += Simplex(product_index(p1[start1], p2[start2]));
  return result.join(create_staircase_triang(p1, p2, start1 + 1, start2)
		     + create_staircase_triang(p1, p2, start1, start2 + 1));
}

SimplicialComplex create_triang(const HashSet<Permutation>& h) {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing triangulation T of A" << std::endl;
  }
  SimplicialComplex result;
  for (HashSet<Permutation>::const_iterator iter = h.begin();
       iter != h.end();
       ++iter) {
      result += Simplex(*iter);
  }
  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  return result;
}

SimplicialComplex create_staircase_triang(const HashSet<Permutation>& h1, const HashSet<Permutation>& h2) {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing triangulation stair(T*S) of C" << std::endl
	 << "as staircase refinement of the product of T and S ..." << std::endl;
  }
  SimplicialComplex result;
  for (HashSet<Permutation>::const_iterator iter1 = h1.begin();
       iter1 != h1.end();
       ++iter1) {
    for (HashSet<Permutation>::const_iterator iter2 = h2.begin();
	 iter2 != h2.end();
	 ++iter2) {
      result += (create_staircase_triang(*iter1, *iter2));
    }
  }
  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  return result;
}


int main(int argc, const char** argv) {
  CommandlineOptions::init(argc, argv);

  std::cout << create_C().homogenize() << std::endl;

  std::cout << create_GH() << std::endl;

  std::cout << create_staircase_triang(create_T(), create_S()) << std::endl;

//   std::cout << create_A().homogenize() << std::endl;
  
//   std::cout << create_G() << std::endl;
  
//   std::cout << create_triang(create_T()) << std::endl;

  return 0;
}
