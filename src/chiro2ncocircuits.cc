////////////////////////////////////////////////////////////////////////////////
// 
// chiro2ncocircuits.cc 
//
//    produced: 29/01/2020 jr
// last change: 29/01/2020 jr
//
////////////////////////////////////////////////////////////////////////////////
#include "CommandlineOptions.hh"
#include "ComputeCocircuits.hh"

int main (const int argc, const char** argv) {
  using namespace topcom;

  CommandlineOptions::init(argc, argv);
  return (ComputeCocircuits::run(INPUT_CHIRO));
}
    
// eof chiro2ncocircuits.cc
