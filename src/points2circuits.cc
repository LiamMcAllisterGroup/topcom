////////////////////////////////////////////////////////////////////////////////
// 
// points2circuits.cc 
//
//    produced: 19/07/19 jr
// last change: 19/07/19 jr
//
////////////////////////////////////////////////////////////////////////////////
#include "CommandlineOptions.hh"
#include "ComputeCircuits.hh"

int main (const int argc, const char** argv) {
  using namespace topcom;

  CommandlineOptions::init(argc, argv);
  return (ComputeCircuits::run(OUTPUT_CIRCUITS));
}
 
// eof points2circuits.cc
