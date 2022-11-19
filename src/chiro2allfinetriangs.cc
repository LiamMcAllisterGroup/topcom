#include "CommandlineOptions.hh"
#include "ComputeTriangs.hh"

int main(const int argc, const char** argv) {
  using namespace topcom;

  CommandlineOptions::init(argc, argv);
  return (ComputeTriangs::run(INPUT_CHIRO | COMPUTE_ALL | OUTPUT_TRIANGS | FINE_ONLY));
}

// eof chiro2allfinetriangs.cc
