#include "CommandlineOptions.hh"
#include "ComputeTriangs.hh"

int main(const int argc, const char** argv) {
  CommandlineOptions::init(argc, argv);
  return (ComputeTriangs::run(COMPUTE_ALL));
}
