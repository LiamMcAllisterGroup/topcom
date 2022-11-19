#include "CommandlineOptions.hh"
#include "ComputeTriangs.hh"

int main(const int argc, const char** argv) {
  using namespace topcom;

  CommandlineOptions::init(argc, argv);
  return (ComputeTriangs::run(INPUT_CHIRO | OUTPUT_TRIANGS));
}

// eof chiro2triangs.cc
