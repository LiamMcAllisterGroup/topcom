//////////////////////////////////////////////////////////////////////////
// ComputeTriangs.hh
// produced: 25 Nov 1999 jr
// last change: 25 Nov 1999 jr
/////////////////////////////////////////////////////////////////////////
#ifndef COMPUTETRIANGS_HH
#define COMPUTETRIANGS_HH

namespace topcom {

#define INPUT_CHIRO    0x0001
#define COMPUTE_ALL    0x0002
#define FINE_ONLY      0x0004
#define OUTPUT_TRIANGS 0x0008
#define PREPROCESS     0x0010
#define FINDFLIPS      0x0020
#define UNIMODULAR     0x0040
#define FINDMIN        0x0080

  class ComputeTriangs {
  public:
    static int run(const int flags = 0);
  };

}; // namespace topcom

#endif

// eof ComputeTriangs.hh
