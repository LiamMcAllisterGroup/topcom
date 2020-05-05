#include "CommandlineOptions.hh"
#include "ComputeTriangs.hh"

int main(const int argc, const char** argv) {
  CommandlineOptions::init(argc, argv);
  return (ComputeTriangs::run(INPUT_CHIRO | FINE_ONLY | OUTPUT_TRIANGS));
}
