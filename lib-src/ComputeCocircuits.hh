//////////////////////////////////////////////////////////////////////////
//
// ComputeCocircuits.hh
//
//    produced: 29/01/2020 jr
// last change: 29/01/2020 jr
// 
/////////////////////////////////////////////////////////////////////////
#ifndef COMPUTECOCIRCUITS_HH
#define COMPUTECOCIRCUITS_HH

#include "LabelSet.hh"
#include "HashKey.hh"

namespace topcom {

  typedef LabelSet spanning_set_type;

#ifndef STL_CONTAINERS
#include "PlainHashSet.hh"
#include "HashSet.hh"
  typedef PlainHashSet<spanning_set_type> spanning_sets_type;
#else
#include <unordered_set>
  typedef std::unordered_set<spanning_set_type, Hash<spanning_set_type> > spanning_sets_type;
#endif


#define INPUT_CHIRO       0x0001
#define OUTPUT_COCIRCUITS 0x0008
#define PREPROCESS        0x0010
#define FROM_CHIRO        0x0080

  class ComputeCocircuits {
  public:
    static int  run (const int flags = 0);
  };

}; // namespace topcom

#endif

// eof ComputeCocircuits.hh
