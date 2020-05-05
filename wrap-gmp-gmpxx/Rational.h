////////////////////////////////////////////////////////////////////////////////
// 
// Integer.h
//
//    produced: 2009/04/30 jr
// last change: 2009/04/30 jr
// 
////////////////////////////////////////////////////////////////////////////////
#ifndef _RATIONAL_H
#define _RATIONAL_H

#include <iostream>
#include "gmpxx.h"
#include "Integer.h"

typedef mpq_class Rational;

// add specialized zero-check only depending on numerator (workaround because of error(?) in gmp)
// disabled essentially because error was caused by non-canonicalized input
inline bool is_zero(const Rational& a) { 
#ifdef SUPER_VERBOSE
  std::cerr << "Checking " << a << " for equality with zero." << std::endl;
#endif
  //  return (Integer(a.get_num()) == Integer(0));
  return (a == 0);
};

namespace std_ext {
   template <typename _Key> struct hash;

   template <> struct hash<Rational> {
   public:
      size_t operator() (const Rational& a) const {
	return std_ext::hash<Integer>()(Integer(a.get_num())) ^ std_ext::hash<Integer>()(Integer(a.get_den()));
      }
   };
}

#endif // RATIONAL_H

// Local Variables:
// mode:C++
// End:
