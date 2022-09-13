#include <iostream>

#include "CommandlineOptions.hh"

#include "ContainerIO.hh"
#include "PointConfiguration.hh"
#include "Permutation.hh"
#include "Symmetry.hh"

int main (const int argc, const char** argv) {
  using namespace topcom;

  CommandlineOptions::init(argc, argv);
  if (CommandlineOptions::verbose()) {
    std::cerr << std::endl;
    std::cerr << "---------------------------------------------------\n";
    std::cerr << "computing some symmetries of a point configuration \n";
    std::cerr << "---------------------------------------------------\n";
    std::cerr << std::endl;
  }
  PointConfiguration points;
  if (std::cin >> points) {
    if ((points.no() < 2) || (points.rank() < 2)) {
      if (CommandlineOptions::verbose()) {
	std::cerr << "no of points and rank must be at least two." << std::endl;
      }
      return 1;
    }
    if (points.rank() > points.no()) {
      if (CommandlineOptions::verbose()) {
	std::cerr << "rank must not be larger than no of points." << std::endl;
      }
      return 1;
    }
    symmetry_collectordata S_data;
    Permutation perm(points.rank(), 2);
    do {
      Symmetry s(perm, true);
      S_data.insert(s);
    } while (perm.lexnext());
    SymmetryGroup S(points.rank(), S_data);
    symmetry_collectordata result_generators(S.rowperm_symmetry_generators(points));
    SymmetryGroup result(points.no(), result_generators, false);
    result.write_generators(std::cout);
    std::cout << std::endl;
    return 0;
  }
  else {
    if (CommandlineOptions::verbose()) {
      std::cerr << "points2symmetries: error while reading point configuration."
		<< std::endl;
    }
    return 1;
  }
}

// eof points2symmetries.cc
