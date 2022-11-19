///////////////////////////////////////////////////////////////////////////////
// 
// LabelSet.hh
//
//    produced: 19/11/19 jr
// last change: 19/11/19 jr
// 
////////////////////////////////////////////////////////////////////////////////
#ifndef LABELSET_HH
#define LABELSET_HH

#ifdef MAXNOIS64
#include "IntegerSet64.hh"
namespace topcom {
  typedef IntegerSet64 LabelSet;
};
#else
#include "IntegerSet.hh"
namespace topcom {
  typedef IntegerSet LabelSet;
};
#endif

#ifndef STL_CONTAINERS
#include "Pair.hh"
namespace topcom {
  typedef Pair<LabelSet, LabelSet>      signvector_data;
};
#else
#include "ContainerIO.hh"
namespace topcom {
  typedef std::pair<LabelSet, LabelSet> signvector_data;
};
#endif
  
#endif
// eof LabelSet.hh
