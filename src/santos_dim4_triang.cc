// This program contains the construction of a derived 4d-triangulation
// based on
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
#include "HashSet.hh"

namespace topcom {

PointConfiguration create_a() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing a ..." << std::endl;
  }
  PointConfiguration result(Matrix(4, 3, FieldConstants::ZERO));
  result[0][0] = Field(9);
  result[1][1] = Field(9);
  result[2][2] = Field(9);
  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  return result;
}

PointConfiguration create_b() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing b ..." << std::endl;
  }
  PointConfiguration result(Matrix(4, 3, FieldConstants::ZERO));
  result[0][0] = Field(1);
  result[0][1] = Field(4);
  result[0][2] = Field(4);
  result[1][0] = Field(4);
  result[1][1] = Field(1);
  result[1][2] = Field(4);
  result[2][0] = Field(4);
  result[2][1] = Field(4);
  result[2][2] = Field(1);
  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  return result;
}

PointConfiguration create_o() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing o ..." << std::endl;
  }
  PointConfiguration result(Matrix(4, 2, FieldConstants::ZERO));
  result[0][0] = Field(3);
  result[0][1] = Field(3);
  result[0][2] = Field(3);
  result[0][3] = Field(9);
  result[1][0] = Field(3);
  result[1][1] = Field(3);
  result[1][2] = Field(3);
  result[1][3] = Field(-9);
  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  return result;
}

PointConfiguration create_c1() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing c ..." << std::endl;
  }
  PointConfiguration result(Matrix(4, 3, FieldConstants::ZERO));

  CyclicIndex cyclic_index(3,3); 

  result[cyclic_index(0,1)][0] = Field(26) * Field(3) / Field(17);
  result[cyclic_index(0,1)][1] = Field(11) * Field(3) / Field(17);
  result[cyclic_index(0,1)][2] = Field(14) * Field(3) / Field(17);
  result[cyclic_index(0,1)][3] = Field(-30) * Field(3) / Field(17);

  result[cyclic_index(1,2)][0] = Field(14) * Field(3) / Field(17);
  result[cyclic_index(1,2)][1] = Field(26) * Field(3) / Field(17);
  result[cyclic_index(1,2)][2] = Field(11) * Field(3) / Field(17);
  result[cyclic_index(1,2)][3] = Field(-30) * Field(3) / Field(17);

  result[cyclic_index(2,0)][0] = Field(11) * Field(3) / Field(17);
  result[cyclic_index(2,0)][1] = Field(14) * Field(3) / Field(17);
  result[cyclic_index(2,0)][2] = Field(26) * Field(3) / Field(17);
  result[cyclic_index(2,0)][3] = Field(-30) * Field(3) / Field(17);

  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  return result;
}

PointConfiguration create_c2() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing c2 ..." << std::endl;
  }
  PointConfiguration result(Matrix(4, 3, FieldConstants::ZERO));

  RevCyclicIndex rev_cyclic_index(3,3); 

  result[rev_cyclic_index(1,0)][0] = Field(11) * Field(3) / Field(17);
  result[rev_cyclic_index(1,0)][1] = Field(26) * Field(3) / Field(17);
  result[rev_cyclic_index(1,0)][2] = Field(14) * Field(3) / Field(17);
  result[rev_cyclic_index(1,0)][3] = Field(30) * Field(3) / Field(17);

  result[rev_cyclic_index(2,1)][0] = Field(14) * Field(3) / Field(17);
  result[rev_cyclic_index(2,1)][1] = Field(11) * Field(3) / Field(17);
  result[rev_cyclic_index(2,1)][2] = Field(26) * Field(3) / Field(17);
  result[rev_cyclic_index(2,1)][3] = Field(30) * Field(3) / Field(17);

  result[rev_cyclic_index(0,2)][0] = Field(26) * Field(3) / Field(17);
  result[rev_cyclic_index(0,2)][1] = Field(14) * Field(3) / Field(17);
  result[rev_cyclic_index(0,2)][2] = Field(11) * Field(3) / Field(17);
  result[rev_cyclic_index(0,2)][3] = Field(30) * Field(3) / Field(17);

  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  return result;
}

PointConfiguration create_A() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing three-dimensional point configuration A from a, b, o, c1, c2 ..." 
	 << std::endl;
  }
  Matrix result;
  result.augment(create_a());
  result.augment(create_b());
  result.augment(create_o());
  result.augment(create_c1());
  result.augment(create_c2());
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
  Matrix result(1, 2, FieldConstants::ZERO);
  result[0][0] = FieldConstants::MINUSONE;
  result[1][0] = FieldConstants::ONE;
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

Symmetry create_g1() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing g1 ..." << std::endl;
  }
  Symmetry result(14);
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
  for (size_type i = 11; i < 14; ++i) {
    result[i] = ((i - 11) + 1) % 3 + 11;
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
  Symmetry result(14);
  result[0] = 1;
  result[1] = 0;
  result[2] = 2;

  result[3] = 4;
  result[4] = 3;
  result[5] = 5;

  result[6] = 7;
  result[7] = 6;

  result[8] = 11;
  result[9] = 13;
  result[10] = 12;

  result[11] = 8;
  result[12] = 10;
  result[13] = 9;

  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  return result;
}

Symmetry create_h1() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing h1 ..." << std::endl;
  }
  Symmetry result(28);
  result[0] = 0;
  for (size_type i = 0; i < 6; ++i) {
    result[i] = (i + 2) % 6;
  }
  for (size_type i = 6; i < 12; ++i) {
    result[i] = ((i - 6) + 2) % 6 + 6;
  }
  for (size_type i = 12; i < 16; ++i) {
    result[i] = i;
  }
  for (size_type i = 16; i < 22; ++i) {
    result[i] = ((i - 16) + 2) % 6 + 16;
  }
  for (size_type i = 22; i < 28; ++i) {
    result[i] = ((i - 22) + 2) % 6 + 22;
  }
  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  return result;
}

Symmetry create_h2() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing h2 ..." << std::endl;
  }
  Symmetry result(28);
  result[0] = 3;
  result[1] = 2;
  result[2] = 1;
  result[3] = 0;
  result[4] = 5;
  result[5] = 4;

  result[6] = 9;
  result[7] = 8;
  result[8] = 7;
  result[9] = 6;
  result[10] = 11;
  result[11] = 10;

  result[12] = 15;
  result[13] = 14;
  result[14] = 13;
  result[15] = 12;

  result[16] = 23;
  result[17] = 22;
  result[18] = 27;
  result[19] = 26;
  result[20] = 25;
  result[21] = 24;

  result[22] = 17;
  result[23] = 16;
  result[24] = 21;
  result[25] = 20;
  result[26] = 19;
  result[27] = 18;

  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  return result;
}

SymmetryGroup create_G1() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing symmetry group G1 of A from g1 ..." 
	 << std::endl;
  }
  symmetry_collectordata result;
  result.insert(create_g1());
  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  return SymmetryGroup(14, result);
}

SymmetryGroup create_G() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing symmetry group G of A from g1, g2 ..." 
	 << std::endl;
  }
  symmetry_collectordata result;
  result.insert(create_g1());
//   result.insert(create_g2());
  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  return SymmetryGroup(14, result);
}

SymmetryGroup create_H() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing symmetry group H of C from h1, h2 ..." 
	 << std::endl;
  }
  symmetry_collectordata result;
  result.insert(create_h1());
  result.insert(create_h2());
  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  return SymmetryGroup(28, result);
}

Permutation create_A1() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing A1 ..." << std::endl;
  }
  Permutation result(14,4);
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
  Permutation result(14,4);
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
  Permutation result(14,4);
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
  Permutation result(14,4);
  result[0] = 8;
  result[1] = 7;
  result[2] = 0;
  result[3] = 2;

  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  return result;
}

Permutation create_B4() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing B1 ..." << std::endl;
  }
  Permutation result(14,4);
  result[0] = 1;
  result[1] = 5;
  result[2] = 0;
  result[3] = 13;

  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  return result;
}

Permutation create_B5() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing B2 ..." << std::endl;
  }
  Permutation result(14,4);
  result[0] = 1;
  result[1] = 0;
  result[2] = 6;
  result[3] = 13;

  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  return result;
}

Permutation create_B6() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing B3 ..." << std::endl;
  }
  Permutation result(14,4);
  result[0] = 1;
  result[1] = 6;
  result[2] = 5;
  result[3] = 13;

  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  return result;
}

Permutation create_C1() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing C1 ..." << std::endl;
  }
  Permutation result(14,4);
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
  Permutation result(14,4);
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
  Permutation result(14,4);
  result[0] = 8;
  result[1] = 5;
  result[2] = 7;
  result[3] = 0;

  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  return result;
}

Permutation create_C4() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing C1 ..." << std::endl;
  }
  Permutation result(14,4);
  result[0] = 5;
  result[1] = 0;
  result[2] = 4;
  result[3] = 13;

  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  return result;
}

Permutation create_C5() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing C2 ..." << std::endl;
  }
  Permutation result(14,4);
  result[0] = 0;
  result[1] = 6;
  result[2] = 4;
  result[3] = 13;

  if (CommandlineOptions::verbose()) {
    std::cerr << "... done." << std::endl;
  }
  return result;
}

Permutation create_C6() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing C3 ..." << std::endl;
  }
  Permutation result(14,4);
  result[0] = 6;
  result[1] = 5;
  result[2] = 4;
  result[3] = 13;

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
      std::cerr << "inserting " << g.map(s) << " ..." << std::endl;
    }
    result.insert(g.map(s));
    if (CommandlineOptions::debug()) {
      std::cerr << "... done." << std::endl;
    }
  }
  return result;
}

HashSet<Permutation> create_T() {
  if (CommandlineOptions::verbose()) {
    std::cerr << "constructing triangulation T of A" << std::endl
	 << "from A1, B1, B2, B3, C1, C2, C3 ..." 
	 << std::endl;
  }
  HashSet<Permutation> result;
  const SymmetryGroup G(create_G());
  const SymmetryGroup G1(create_G1());
  insert_orbit(create_A1(), G1, result);
  insert_orbit(create_B1(), G, result);
  insert_orbit(create_B2(), G, result);
  insert_orbit(create_B3(), G, result);
  insert_orbit(create_B4(), G, result);
  insert_orbit(create_B5(), G, result);
  insert_orbit(create_B6(), G, result);
  insert_orbit(create_C1(), G, result);
  insert_orbit(create_C2(), G, result);
  insert_orbit(create_C3(), G, result);
  insert_orbit(create_C4(), G, result);
  insert_orbit(create_C5(), G, result);
  insert_orbit(create_C6(), G, result);
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

};

int main(int argc, const char** argv) {
  using namespace topcom;

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
  
  std::cout << create_H() << std::endl;
  
  std::cout << create_staircase_triang(create_T(), create_S()) << std::endl;

  return 0;
}

// eof santos_dim4_triang.cc
