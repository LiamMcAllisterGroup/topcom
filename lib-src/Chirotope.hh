////////////////////////////////////////////////////////////////////////////////
// 
// Chirotope.hh 
//
//    produced: 19 Nov 1999 jr
// last change: 19 Nov 1999 jr
//
////////////////////////////////////////////////////////////////////////////////
#ifndef CHIROTOPE_HH
#define CHIROTOPE_HH

#include "RealChiro.hh"
#include "VirtualChiro.hh"

namespace topcom {
  
#ifndef PREPROCESS_CHIRO
  typedef VirtualChiro Chirotope;
#else
  typedef RealChiro Chirotope;
#endif
  
}; // namespace topcom

#endif
// eof Chirotope.hh
