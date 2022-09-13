#include <iostream>

#include "CommandlineOptions.hh"

#include "PointConfiguration.hh"

int main (const int argc, const char** argv) {
  using namespace topcom;

  CommandlineOptions::init(argc, argv);
  if (CommandlineOptions::verbose()) {
    std::cerr << std::endl;
    std::cerr << "---------------------------------------------------\n";
    std::cerr << "   computing the volume of a point configuration   \n";
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
    std::cout << points.volume() << std::endl;
    return 0;
  }
  else {
    if (CommandlineOptions::verbose()) {
      std::cerr << "points2volume: error while reading point configuration."
		<< std::endl;
    }
    return 1;
  }
}

// eof points2volume.cc
