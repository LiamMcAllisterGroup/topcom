////////////////////////////////////////////////////////////////////////////////
// 
// chiro2ncircuits.cc 
//
//    produced: 16/07/19 jr
// last change: 17/07/19 jr
//
////////////////////////////////////////////////////////////////////////////////
#include "CommandlineOptions.hh"
#include "ComputeCircuits.hh"

int main (const int argc, const char** argv) {
  using namespace topcom;

  CommandlineOptions::init(argc, argv);
  return (ComputeCircuits::run(INPUT_CHIRO));
}
	
// eof chiro2ncircuits.cc
