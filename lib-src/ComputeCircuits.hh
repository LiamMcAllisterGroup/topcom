//////////////////////////////////////////////////////////////////////////
//
// ComputeCircuits.hh
//
//    produced: 16/07/19 jr
// last change: 29/01/2020 jr
//
/////////////////////////////////////////////////////////////////////////
#ifndef COMPUTECIRCUITS_HH
#define COMPUTECIRCUITS_HH

namespace topcom {

#define INPUT_CHIRO     0x0001
#define OUTPUT_CIRCUITS 0x0008
#define PREPROCESS      0x0010
#define FROM_CHIRO      0x0080

  class ComputeCircuits {
  public:
    static int  run (const int flags = 0);
  };

}; // namespace topcom

#endif

// eof ComputeCircuits.hh
