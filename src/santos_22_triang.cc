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

PointConfiguration create_a() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing a ..." << std::endl;
  }
  PointConfiguration result(Matrix(4, 3, ZERO));

  result[0][0] = Field(11);
  result[0][2] = Field(-2);

  result[1][1] = Field(11);
  result[1][0] = Field(-2);

  result[2][2] = Field(11);
  result[2][1] = Field(-2);

  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  return result;
}

PointConfiguration create_b() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing b ..." << std::endl;
  }
  PointConfiguration result(Matrix(4, 3, ZERO));

  result[0][0] = Field(2);
  result[0][1] = Field(2);
  result[0][2] = Field(5);
  result[0][3] = Field(-2);

  result[1][1] = Field(2);
  result[1][2] = Field(2);
  result[1][0] = Field(5);
  result[1][3] = Field(-2);

  result[2][2] = Field(2);
  result[2][0] = Field(2);
  result[2][1] = Field(5);
  result[2][3] = Field(-2);

  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  return result;
}

PointConfiguration create_o() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing o ..." << std::endl;
  }
  PointConfiguration result(Matrix(4, 2, ZERO));

  result[0][0] = Field(3);
  result[0][1] = Field(3);
  result[0][2] = Field(3);
  result[0][3] = Field(0);

  result[1][0] = Field(3);
  result[1][1] = Field(3);
  result[1][2] = Field(3);
  result[1][3] = Field(-3);

  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  return result;
}

PointConfiguration create_c() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing c ..." << std::endl;
  }
  PointConfiguration result(Matrix(4, 3, ZERO));

  CyclicIndex cyclic_index(3,3); 

  result[cyclic_index(0,1)][0] = Field(11) / Field(2);
  result[cyclic_index(0,1)][1] = Field(2);
  result[cyclic_index(0,1)][2] = Field(3) / Field(2);
  result[cyclic_index(0,1)][3] = Field(-2);

  result[cyclic_index(1,2)][1] = Field(11) / Field(2);
  result[cyclic_index(1,2)][2] = Field(2);
  result[cyclic_index(1,2)][0] = Field(3) / Field(2);
  result[cyclic_index(1,2)][3] = Field(-2);

  result[cyclic_index(2,0)][2] = Field(11) / Field(2);
  result[cyclic_index(2,0)][0] = Field(2);
  result[cyclic_index(2,0)][1] = Field(3) / Field(2);
  result[cyclic_index(2,0)][3] = Field(-2);

  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  return result;
}

PointConfiguration create_A() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing three-dimensional point configuration A from a, b, o, c, ..." 
	 << std::endl;
  }
  Matrix result;
  result.augment(create_a());
  result.augment(create_b());
  result.augment(create_o());
  result.augment(create_c());
  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  return PointConfiguration(result);
}

PointConfiguration create_B() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing one-dimensional point configuration B ..." 
	 << std::endl;
  }
  Matrix result(1, 2, ZERO);
  result[0][0] = MINUSONE;
  result[1][0] = ONE;
  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  return PointConfiguration(result);
}

PointConfiguration create_C() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing four-dimensional point configuration C from A and B ..." 
	 << std::endl;
  }
  PointConfiguration A(create_A());
  PointConfiguration B(create_B());
  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  return A.product(B);
}

Symmetry create_g() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing g ..." << std::endl;
  }
  Symmetry result(11);
  result[0] = 0;
  for (size_type i = 0; i < 3; ++i) {
    result[i] = (i + 1) % 3;
  }
  for (size_type i = 3; i < 6; ++i) {
    result[i] = ((i - 3) + 1) % 3 + 3;
  }
  for (size_type i = 6; i < 8; ++i) {
    result[i] = i;
  }
  for (size_type i = 8; i < 11; ++i) {
    result[i] = ((i - 8) + 1) % 3 + 8;
  }
  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  return result;
}

SymmetryGroup create_G() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing symmetry group G1 of A from g1 ..." 
	 << std::endl;
  }
  symmetry_data result;
  result.insert(create_g());
  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  return SymmetryGroup(11, result);
}

SymmetryGroup create_H() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing symmetry group H of B (trivial) ..." 
	 << std::endl;
  }
  symmetry_data result;
  result.insert(Symmetry(2));
  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  return SymmetryGroup(2, result);
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

Permutation create_A1() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing A1 ..." << std::endl;
  }
  Permutation result(11,4);
  result[0] = 6;
  result[1] = 4;
  result[2] = 3;
  result[3] = 7;

  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  return result;
}

Permutation create_B1() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing B1 ..." << std::endl;
  }
  Permutation result(11,4);
  result[0] = 8;
  result[1] = 0;
  result[2] = 4;
  result[3] = 2;

  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  return result;
}

Permutation create_B2() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing B2 ..." << std::endl;
  }
  Permutation result(11,4);
  result[0] = 8;
  result[1] = 4;
  result[2] = 7;
  result[3] = 2;

  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  return result;
}

Permutation create_B3() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing B3 ..." << std::endl;
  }
  Permutation result(11,4);
  result[0] = 8;
  result[1] = 7;
  result[2] = 0;
  result[3] = 2;

  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  return result;
}

Permutation create_C1() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing C1 ..." << std::endl;
  }
  Permutation result(11,4);
  result[0] = 8;
  result[1] = 5;
  result[2] = 0;
  result[3] = 4;

  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  return result;
}

Permutation create_C2() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing C2 ..." << std::endl;
  }
  Permutation result(11,4);
  result[0] = 8;
  result[1] = 5;
  result[2] = 4;
  result[3] = 7;

  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  return result;
}

Permutation create_C3() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing C3 ..." << std::endl;
  }
  Permutation result(11,4);
  result[0] = 8;
  result[1] = 5;
  result[2] = 7;
  result[3] = 0;

  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  return result;
}

Permutation create_D1() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing D1 ..." << std::endl;
  }
  Permutation result(11,4);
  result[0] = 0;
  result[1] = 2;
  result[2] = 6;
  result[3] = 3;

  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  return result;
}

Permutation create_D2() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing D2 ..." << std::endl;
  }
  Permutation result(11,4);
  result[0] = 0;
  result[1] = 6;
  result[2] = 4;
  result[3] = 3;

  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  return result;
}

Permutation create_D3() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing D3 ..." << std::endl;
  }
  Permutation result(11,4);
  result[0] = 0;
  result[1] = 4;
  result[2] = 2;
  result[3] = 3;

  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  return result;
}

HashSet<Permutation> insert_orbit(const Permutation& s, const SymmetryGroup& G, 
				  HashSet<Permutation>& result) {
  if (CommandlineOptions::debug()) {
    std::cerr << "inserting " << s << " ..." << std::endl;
  }
  result.insert(s);
  if (CommandlineOptions::debug()) {
    std::cerr << "... done." << std::endl;
  }
  for (SymmetryGroup::const_iterator iter = G.begin();
       iter != G.end();
       ++iter) {
#ifdef NO_STL
    const Symmetry& g(iter->key());
#else
    const Symmetry& g(*iter);
#endif
    
    if (CommandlineOptions::debug()) {
      std::cerr << "inserting " << g(s) << " ..." << std::endl;
    }
    result.insert(g(s));
    if (CommandlineOptions::debug()) {
      std::cerr << "... done." << std::endl;
    }
  }
  return result;
}

HashSet<Permutation> create_T() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing triangulation T of A" << std::endl
	 << "from A1, B1, B2, B3, C1, C2, C3, D1, D2, D3 ..." 
	 << std::endl;
  }
  HashSet<Permutation> result;
  const SymmetryGroup G(create_G());
  insert_orbit(create_A1(), G, result);
  insert_orbit(create_B1(), G, result);
  insert_orbit(create_B2(), G, result);
  insert_orbit(create_B3(), G, result);
  insert_orbit(create_C1(), G, result);
  insert_orbit(create_C2(), G, result);
  insert_orbit(create_C3(), G, result);
  insert_orbit(create_D1(), G, result);
  insert_orbit(create_D2(), G, result);
  insert_orbit(create_D3(), G, result);
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
  Permutation s1(2, 2);
  s1[0] = 0;
  s1[1] = 1;
  result.insert(s1);
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

  //  create_C().pretty_print(std::cout);


  if (argc > 1) {
    if (strcmp(argv[1],"-3") == 0) {

      std::cout << create_A() << std::endl;
      
      std::cout << create_G() << std::endl;
      
      std::cout << create_triang(create_T()) << std::endl;

      return 0;
    }
  }
  
  std::cout << create_C() << std::endl;
  
  std::cout << create_GH() << std::endl;
  
  std::cout << create_staircase_triang(create_T(), create_S()) << std::endl;

  return 0;
}
