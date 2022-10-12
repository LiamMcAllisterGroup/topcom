#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>

#include "PermlibPermutationGroup.hh"
#include "SimplicialComplex.hh"
#include "Symmetry.hh"
#include "SwitchTable.hh"

#ifdef HAVE_PERMLIB
topcom::IntegerSet PermlibToIntegerSet(const topcom::parameter_type n, const permlib::dset ds) {
  topcom::IntegerSet result;
  for (unsigned int i = 0; i < n; ++i) {
    if (ds[i]) {
      result += i;
    }
  }
  return result;
}
#endif

int main (const int argc, const char** argv) {
  
  topcom::CommandlineOptions::init(argc, argv);

  topcom::symmetry_collectordata collector;
  topcom::parameter_type n = 4;

  topcom::LabelSet ls1(std::vector<topcom::parameter_type> {31, 30, 29, 28, 27, 26});
  topcom::LabelSet ls2(std::vector<topcom::parameter_type> {31, 30, 29, 28, 1, 0});
  
  // topcom::Permutation cycle(n, 2);
  // auto g1 = topcom::Symmetry(cycle, true);
  // collector.insert(g1);
  // cycle.lexnext();
  // auto g2 = topcom::Symmetry(cycle, true);
  // // collector.insert(g2);
  // cycle.lexnext();
  // auto g3 = topcom::Symmetry(cycle, true);
  // // collector.insert(g3);
  // cycle.lexnext();
  // auto g4 = topcom::Symmetry(cycle, true);
  // collector.insert(g4);

  // new example in the 5-cube:
  n = 32;
  
  // group generators of the 5-cube:
  topcom::Symmetry gen1 = topcom::Symmetry(n, {0,16,8,24,4,20,12,28,2,18,10,26,6,22,14,30,1,17,9,25,5,21,13,29,3,19,11,27,7,23,15,31});
  topcom::Symmetry gen2 = topcom::Symmetry(n, {24,28,26,30,25,29,27,31,16,20,18,22,17,21,19,23,8,12,10,14,9,13,11,15,0,4,2,6,1,5,3,7});
  topcom::Symmetry gen3 = topcom::Symmetry(n, {28,30,29,31,24,26,25,27,20,22,21,23,16,18,17,19,12,14,13,15,8,10,9,11,4,6,5,7,0,2,1,3});
  topcom::Symmetry gen4 = topcom::Symmetry(n, {16,24,20,28,18,26,22,30,17,25,21,29,19,27,23,31,0,8,4,12,2,10,6,14,1,9,5,13,3,11,7,15});
  topcom::Symmetry gen5 = topcom::Symmetry(n, {30,31,28,29,26,27,24,25,22,23,20,21,18,19,16,17,14,15,12,13,10,11,8,9,6,7,4,5,2,3,0,1});
  topcom::Symmetry gen6 = topcom::Symmetry(n, {31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0});
  
  collector.insert(gen1);
  collector.insert(gen2);
  collector.insert(gen3);
  collector.insert(gen4);
  collector.insert(gen5);
  collector.insert(gen6);
  
  std::cout << "generators:\n" << collector << std::endl;

  topcom::SymmetryGroup G(n, collector, true);

  std::cout << "built group with " << G.order() << " elements" << std::endl;

  // topcom::SwitchTable<topcom::lexmin_mode> st(G);
  topcom::SwitchTable<topcom::LabelSet, topcom::colexmax_mode> stg(n, collector);

  // std::cout << "switch table from group:\n" << st << std::endl;
  std::cout << "switch table from generators:\n" << stg << std::endl;
  std::cout << "switch table order:\n" << stg.order() << std::endl;

  // topcom::Symmetry g1g2(g1 * g2);
  // topcom::Symmetry g2g1(g2 * g1);
  // topcom::Symmetry g1g3(g1 * g3);
  // topcom::Symmetry g3g1(g3 * g1);
  // topcom::Symmetry g2g3(g2 * g3);
  // topcom::Symmetry g3g2(g3 * g2);
  
  // std::cout << "membership with switch table from group:" << std::endl;
  // std::cout << "symmetry g1 = " << g1 << " in G?" << std::endl;
  // std::cout << std::boolalpha << st.is_in_group(g1) << std::endl;
  // std::cout << "symmetry g2 = " << g2 << " in G?" << std::endl;
  // std::cout << std::boolalpha << st.is_in_group(g2) << std::endl;
  // std::cout << "symmetry g3 = " << g3 << " in G?" << std::endl;
  // std::cout << std::boolalpha << st.is_in_group(g3) << std::endl;
  // std::cout << "symmetry g4 = " << g4 << " in G?" << std::endl;
  // std::cout << std::boolalpha << st.is_in_group(g4) << std::endl;
  // std::cout << "symmetry g1 * g2 = " << g1g2 << " in G?" << std::endl;
  // std::cout << std::boolalpha << st.is_in_group(g1g2) << std::endl;
  // std::cout << "symmetry g2 * g1 = " << g2g1 << " in G?" << std::endl;
  // std::cout << std::boolalpha << st.is_in_group(g2g1) << std::endl;
  // std::cout << "symmetry g1 * g3 = " << g1g3 << " in G?" << std::endl;
  // std::cout << std::boolalpha << st.is_in_group(g1g3) << std::endl;
  // std::cout << "symmetry g3 * g1 = " << g3g1 << " in G?" << std::endl;
  // std::cout << std::boolalpha << st.is_in_group(g3g1) << std::endl;
  // std::cout << "symmetry g2 * g3 = " << g2g3 << " in G?" << std::endl;
  // std::cout << std::boolalpha << st.is_in_group(g2g3) << std::endl;
  // std::cout << "symmetry g3 * g2 = " << g3g2 << " in G?" << std::endl;
  // std::cout << std::boolalpha << st.is_in_group(g3g2) << std::endl;

  // std::cout << "membership with switch table from generators:" << std::endl;
  // std::cout << "symmetry g1 = " << g1 << " in G?" << std::endl;
  // std::cout << std::boolalpha << stg.is_in_group(g1) << std::endl;
  // std::cout << "symmetry g2 = " << g2 << " in G?" << std::endl;
  // std::cout << std::boolalpha << stg.is_in_group(g2) << std::endl;
  // std::cout << "symmetry g3 = " << g3 << " in G?" << std::endl;
  // std::cout << std::boolalpha << stg.is_in_group(g3) << std::endl;
  // std::cout << "symmetry g4 = " << g4 << " in G?" << std::endl;
  // std::cout << std::boolalpha << stg.is_in_group(g4) << std::endl;
  // std::cout << "symmetry g1 * g2 = " << g1g2 << " in G?" << std::endl;
  // std::cout << std::boolalpha << stg.is_in_group(g1g2) << std::endl;
  // std::cout << "symmetry g2 * g1 = " << g2g1 << " in G?" << std::endl;
  // std::cout << std::boolalpha << stg.is_in_group(g2g1) << std::endl;
  // std::cout << "symmetry g1 * g3 = " << g1g3 << " in G?" << std::endl;
  // std::cout << std::boolalpha << stg.is_in_group(g1g3) << std::endl;
  // std::cout << "symmetry g3 * g1 = " << g3g1 << " in G?" << std::endl;
  // std::cout << std::boolalpha << stg.is_in_group(g3g1) << std::endl;
  // std::cout << "symmetry g2 * g3 = " << g2g3 << " in G?" << std::endl;
  // std::cout << std::boolalpha << stg.is_in_group(g2g3) << std::endl;
  // std::cout << "symmetry g3 * g2 = " << g3g2 << " in G?" << std::endl;
  // std::cout << std::boolalpha << stg.is_in_group(g3g2) << std::endl;

  topcom::size_type no_of_runs = 100000;
  std::cout << no_of_runs << " times non-canonicity with SwitchTable:" << std::endl;
  std::cout << "label set " << ls1 << " not canonical in its orbit?" << std::endl;
  std::cout << "label set " << ls2 << " not canonical in its orbit?" << std::endl;
  bool result1;
  bool result2;
  for (int i = 0; i < no_of_runs; ++i) {
    result1 =  stg.colex_increases(ls1);
    result2 =  stg.colex_increases(ls2);
  }
  std::cout << std::boolalpha << result1 << std::endl;
  std::cout << std::boolalpha << result2 << std::endl;

  std::cout << "order of setwise stabilizer of " << ls1 << ": " << stg.stabilizer_card(ls1) << std::endl;
  std::cout << "order of setwise stabilizer of " << ls2 << ": " << stg.stabilizer_card(ls2) << std::endl;

#ifdef HAVE_PERMLIB
  // some PermLib checks:
  std::cout << std::endl;

  // with the wrapper class:
  topcom::PermlibPermutationGroup<topcom::colexmax_mode> PLPG(n, G.generators());
  std::cout << "group based on PermlibPermutationGroup:\n" << PLPG << std::endl;
  
  // std::cout << "membership with wrapped Permlib:" << std::endl;
  // std::cout << "symmetry g1 = " << g1 << " in G?" << std::endl;
  // std::cout << std::boolalpha << PLPG.is_in_group(g1) << std::endl;
  // std::cout << "symmetry g2 = " << g2 << " in G?" << std::endl;
  // std::cout << std::boolalpha << PLPG.is_in_group(g2) << std::endl;
  // std::cout << "symmetry g3 = " << g3 << " in G?" << std::endl;
  // std::cout << std::boolalpha << PLPG.is_in_group(g3) << std::endl;
  // std::cout << "symmetry g4 = " << g4 << " in G?" << std::endl;
  // std::cout << std::boolalpha << PLPG.is_in_group(g4) << std::endl;
  // std::cout << "symmetry g1 * g2 = " << g1g2 << " in G?" << std::endl;
  // std::cout << std::boolalpha << PLPG.is_in_group(g1g2) << std::endl;
  // std::cout << "symmetry g2 * g1 = " << g2g1 << " in G?" << std::endl;
  // std::cout << std::boolalpha << PLPG.is_in_group(g2g1) << std::endl;
  // std::cout << "symmetry g1 * g3 = " << g1g3 << " in G?" << std::endl;
  // std::cout << std::boolalpha << PLPG.is_in_group(g1g3) << std::endl;
  // std::cout << "symmetry g3 * g1 = " << g3g1 << " in G?" << std::endl;
  // std::cout << std::boolalpha << PLPG.is_in_group(g3g1) << std::endl;
  // std::cout << "symmetry g2 * g3 = " << g2g3 << " in G?" << std::endl;
  // std::cout << std::boolalpha << PLPG.is_in_group(g2g3) << std::endl;
  // std::cout << "symmetry g3 * g2 = " << g3g2 << " in G?" << std::endl;
  // std::cout << std::boolalpha << PLPG.is_in_group(g3g2) << std::endl;

  std::cout << no_of_runs << " times non-canonicity with PermlibPermutationGroup:" << std::endl;
  std::cout << "label set " << ls1 << " not canonical in its orbit?" << std::endl;
  std::cout << "label set " << ls2 << " not canonical in its orbit?" << std::endl;
  for (int i = 0; i < no_of_runs; ++i) {
    result1 =  PLPG.colex_increases(ls1);
    result2 =  PLPG.colex_increases(ls2);
  }
  std::cout << std::boolalpha << result1 << std::endl;
  std::cout << std::boolalpha << result2 << std::endl;

  std::cout << "setwise stabilizer of " << ls1 << ": " << PLPG.stabilizer(ls1) << std::endl;
  std::cout << "setwise stabilizer of " << ls2 << ": " << PLPG.stabilizer(ls2) << std::endl;

  // for (topcom::parameter_type i = 0; i < n; ++i) {
  //   std::cout << "orbit of " << i << " is " << PLPG.point_orbit(i) << std::endl;
  // }

 
  // direct calls:
  // permlib::Permutation::ptr gg1(new permlib::Permutation(g1));
  // permlib::Permutation::ptr gg2(new permlib::Permutation(g2));
  // permlib::Permutation::ptr gg3(new permlib::Permutation(g3));
  // permlib::Permutation::ptr gg4(new permlib::Permutation(g4));

  // std::list<permlib::Permutation::ptr> GGgen;
  // GGgen.push_back(gg1);
  // // GGgen.push_back(gg2);
  // // GGgen.push_back(gg3);
  // GGgen.push_back(gg4);

  // boost::shared_ptr<permlib::PermutationGroup> GG = construct(n, GGgen.begin(), GGgen.end());
  // std::cout << "Group based on Permlib:\n" << *GG << std::endl;

  // permlib::Permutation::ptr gg1gg2(new permlib::Permutation(*gg2 * *gg1));
  // permlib::Permutation::ptr gg2gg1(new permlib::Permutation(*gg1 * *gg2));
  // permlib::Permutation::ptr gg1gg3(new permlib::Permutation(*gg3 * *gg1));
  // permlib::Permutation::ptr gg3gg1(new permlib::Permutation(*gg1 * *gg3));
  // permlib::Permutation::ptr gg2gg3(new permlib::Permutation(*gg3 * *gg2));
  // permlib::Permutation::ptr gg3gg2(new permlib::Permutation(*gg2 * *gg3));

  // std::cout << "membership with switch table from group:" << std::endl;
  // std::cout << "symmetry g1 = " << *gg1 << " in G?" << std::endl;
  // std::cout << std::boolalpha << GG->sifts(*gg1) << std::endl;
  // std::cout << "symmetry g2 = " << *gg2 << " in G?" << std::endl;
  // std::cout << std::boolalpha << GG->sifts(*gg2) << std::endl;
  // std::cout << "symmetry g3 = " << *gg3 << " in G?" << std::endl;
  // std::cout << std::boolalpha << GG->sifts(*gg3) << std::endl;
  // std::cout << "symmetry g4 = " << *gg4 << " in G?" << std::endl;
  // std::cout << std::boolalpha << GG->sifts(*gg4) << std::endl;
  // std::cout << "symmetry g1 * g2 = " << *gg1gg2 << " in G?" << std::endl;
  // std::cout << std::boolalpha << GG->sifts(*gg1gg2) << std::endl;
  // std::cout << "symmetry g2 * g1 = " << *gg2gg1 << " in G?" << std::endl;
  // std::cout << std::boolalpha << GG->sifts(*gg2gg1) << std::endl;
  // std::cout << "symmetry g1 * g3 = " << *gg1gg3 << " in G?" << std::endl;
  // std::cout << std::boolalpha << GG->sifts(*gg1gg3) << std::endl;
  // std::cout << "symmetry g3 * g1 = " << *gg3gg1 << " in G?" << std::endl;
  // std::cout << std::boolalpha << GG->sifts(*gg3gg1) << std::endl;
  // std::cout << "symmetry g2 * g3 = " << *gg2gg3 << " in G?" << std::endl;
  // std::cout << std::boolalpha << GG->sifts(*gg2gg3) << std::endl;
  // std::cout << "symmetry g3 * g2 = " << *gg3gg2 << " in G?" << std::endl;
  // std::cout << std::boolalpha << GG->sifts(*gg3gg2) << std::endl;

  // // new example in the 5-cube:
  // n = 32;
  
  // // group generators of the 5-cube:
  // std::list<permlib::Permutation::ptr> groupGenerators;
  
  // permlib::Permutation::perm gen1raw = {0,16,8,24,4,20,12,28,2,18,10,26,6,22,14,30,1,17,9,25,5,21,13,29,3,19,11,27,7,23,15,31};
  // permlib::Permutation::perm gen2raw = {24,28,26,30,25,29,27,31,16,20,18,22,17,21,19,23,8,12,10,14,9,13,11,15,0,4,2,6,1,5,3,7};
  // permlib::Permutation::perm gen3raw = {28,30,29,31,24,26,25,27,20,22,21,23,16,18,17,19,12,14,13,15,8,10,9,11,4,6,5,7,0,2,1,3};
  // permlib::Permutation::perm gen4raw = {16,24,20,28,18,26,22,30,17,25,21,29,19,27,23,31,0,8,4,12,2,10,6,14,1,9,5,13,3,11,7,15};
  // permlib::Permutation::perm gen5raw = {30,31,28,29,26,27,24,25,22,23,20,21,18,19,16,17,14,15,12,13,10,11,8,9,6,7,4,5,2,3,0,1};
  // permlib::Permutation::perm gen6raw = {31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0};
  
  // permlib::Permutation::ptr gen1(new permlib::Permutation(gen1raw));
  // permlib::Permutation::ptr gen2(new permlib::Permutation(gen2raw));
  // permlib::Permutation::ptr gen3(new permlib::Permutation(gen3raw));
  // permlib::Permutation::ptr gen4(new permlib::Permutation(gen4raw));
  // permlib::Permutation::ptr gen5(new permlib::Permutation(gen5raw));
  // permlib::Permutation::ptr gen6(new permlib::Permutation(gen6raw));
	
  // groupGenerators.push_back(gen1);
  // groupGenerators.push_back(gen2);
  // groupGenerators.push_back(gen3);
  // groupGenerators.push_back(gen4);
  // groupGenerators.push_back(gen5);
  // groupGenerators.push_back(gen6);
  
  // //
  // // EXAMPLE 0 (BSGS): construct a base with strong generating set
  // //
  // boost::shared_ptr<permlib::PermutationGroup> group = construct(n, groupGenerators.begin(), groupGenerators.end());
  // std::cout << "Group " << *group << std::endl;
  
  // // size of our set(s) in this example
  // const unsigned long DeltaSize = 4;
  // // represents the set {1,5,8,9}, translated by -1 as the elements of the domain are 0-based
  // const unsigned long Delta[DeltaSize] = {0, 4, 7, 8};
  
  // //
  // // EXAMPLE 1 (SET STAB): compute a set stabilizer
  // //
  // boost::shared_ptr<permlib::PermutationGroup> stabilizer = setStabilizer(*group, Delta, Delta+DeltaSize);
  // std::cout << "Stabilizer of {1,5,8,9}: " << *stabilizer << std::endl;

  // //
  // // EXAMPLE 2 (SET IMAGE): find elements mapping one set onto another 
  // //
  // const unsigned long Gamma[DeltaSize] = {2, 6, 0, 9};
  // permlib::Permutation::ptr repr = setImage(*group, Delta, Delta+DeltaSize, Gamma, Gamma+DeltaSize);
  // if (repr)
  //   std::cout << "Group element mapping {1,5,8,9} to {1,3,7,10}: " << *repr << std::endl;
  // else
  //   std::cout << "No group element found mapping {1,5,8,9} to {1,3,7,10}." << std::endl;
  
  // const unsigned long Gamma2[DeltaSize] = {2, 6, 10, 9};
  // permlib::Permutation::ptr repr2 = setImage(*group, Delta, Delta+DeltaSize, Gamma2, Gamma2+DeltaSize);
  // if (repr2)
  //   std::cout << "Group element mapping {1,5,8,9} to {3,7,10,11}: " << *repr2 << std::endl;
  // else
  //   std::cout << "No group element found mapping {1,5,8,9} to {3,7,10,11}." << std::endl;
  
  // //
  // // EXAMPLE 3 (ORBTIS): compute orbits of a group
  // //                     in this case: the stabilizer from above
  // //
  // std::list<boost::shared_ptr<permlib::OrbitAsSet> > orbitList = permlib::orbits(*stabilizer);
  // unsigned long orbCount = 1;
  // BOOST_FOREACH(const boost::shared_ptr<permlib::OrbitAsSet>& orbit, orbitList) {
  //   std::cout << "Orbit #" << orbCount << " representative: " << (orbit->element()+1) << std::endl;
  //   ++orbCount;
  // }
  
  // //
  // // EXAMPLE 4 (SMALLEST SET IMAGE): compute lexicographically smallest set of an orbit of sets
  // //
  // // encode Gamma in a 'dset', which is a boost::dynamic_bitset
  // permlib::dset dGamma(n);
  // for (unsigned int i = 0; i < DeltaSize; ++i)
  //   dGamma.set(Gamma[i]);
  // permlib::dset dGammaLeast = permlib::smallestSetImage(*group, dGamma);
  // std::cout << "Lexicographically smallest set in the orbit of {1,3,7,10}:  {";
  // for (unsigned int i = 0; i < n; ++i)
  //   if (dGammaLeast[i])
  //     std::cout << (i+1) << ",";
  // std::cout << "}" << std::endl;
  
  // return 0;
  
#endif
  
  // int cnt = 0;
  
  // IntegerSet i1, i2, i3;

  // std::cout << i1 << std::endl;
  // std::cout << i2 << std::endl;


  // for (int i = 0; i < 2; ++i) {
  //   do {
  //     if (cnt % 5 == 0) {
  // 	i1 += cnt;
  //     }
  //     if (cnt % 7 == 0) {
  // 	i2 += cnt;
  //     }
  //     ++cnt;
  //   } while (cnt < 100);

  //   std::this_thread::sleep_for (std::chrono::seconds(10));
  //   std::cerr << "result set before operation:" << std::endl;
  //   i3.report_internals(std::cerr);
    
  //   i3 = i1 * i2;

  //   std::cerr << "result set after operation:" << std::endl;
  //   i3.report_internals(std::cerr);

  //   std::cerr << "operands after operation:" << std::endl;
  //   i1.report_internals(std::cerr);
  //   i2.report_internals(std::cerr);

  // }
  // std::cout << i1 << std::endl;
  // std::cout << i2 << std::endl;
  // std::cout << i3 << std::endl;

  // SimplicialComplex::preprocess_index_table(8, 4);
  // SimplicialComplex s1, s2, s3, s4, s5, s6, s7, s8, s9, sa, sb;
  // Simplex simp;

  // std::cout << s1.index_set() << std::endl;
  // std::cout << s2.index_set() << std::endl;

  // Permutation perm(16,3);

  // cnt = 0;

  // for (int i = 0; i < 1; ++i) {
  //   do {
  //     if (cnt % 5 == 0) {
  // 	s1.insert(Simplex(perm));
  //     }
  //     if (cnt % 7 == 0) {
  // 	s2.insert(Simplex(perm));
  //     }
  //     ++cnt;
  //   } while (perm.lexnext());
    
  //   std::cout << s1 << std::endl;
  //   std::cout << s2 << std::endl;
  //   std::cout << (s3 = s1 + s2) << std::endl;
  //   std::cout << (s4 = s1 - s2) << std::endl;
  //   std::cout << (s5 = s1 * s2) << std::endl;
  //   std::cout << (s6 = s1 ^ s2) << std::endl;
  //   std::cout << (simp = *s3.begin()) << std::endl;
  //   std::cout << (s7.insert_boundary(simp)) << std::endl;
  //   std::cout << (s8 = s3.link(*s7.begin())) << std::endl;
  //   std::cout << (s9 = s3.star(*s7.begin())) << std::endl;
  //   std::cout << (sa = s2 ^ s8) << std::endl;
  //   std::cout << (sb = sa - s9) << std::endl;

  //   perm = Permutation(16,3);
  //   do {
  //     if (cnt % 10 == 0) {
  // 	s1.erase(Simplex(perm));
  //     }
  //     if (cnt % 21 == 0) {
  // 	s2.erase(Simplex(perm));
  //     }
  //     ++cnt;
  //   } while (perm.lexnext());
   
  //   std::cout << s1 << std::endl;
  //   std::cout << s2 << std::endl;
  //   std::cout << (s3 = s1 + s2) << std::endl;
  //   std::cout << (s4 = s1 - s2) << std::endl;
  //   std::cout << (s5 = s1 * s2) << std::endl;
  //   std::cout << (s6 = s1 ^ s2) << std::endl;
  //   std::cout << (simp = *s3.begin()) << std::endl;
  //   std::cout << (s7.insert_boundary(simp)) << std::endl;
  //   std::cout << (s8 = s3.link(*s7.begin())) << std::endl;
  //   std::cout << (s9 = s3.star(*s7.begin())) << std::endl;
  //   std::cout << (sa = s2 ^ s8) << std::endl;
  //   std::cout << (sb = sa - s9) << std::endl;

  // }
  
  return 0;
}

// eof check.cc
