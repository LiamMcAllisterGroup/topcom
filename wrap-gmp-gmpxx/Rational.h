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

namespace topcom {

  typedef mpq_class Rational;
  
  // add specialized zero-check only depending on numerator (workaround because of error(?) in gmp)
  // disabled essentially because error was caused by non-canonicalized input
  inline bool is_zero(const Rational& a) { 
#ifdef SUPER_VERBOSE
    std::cerr << "checking " << a << " for equality with zero." << std::endl;
#endif
    //  return (Integer(a.get_num()) == Integer(0));
    return (a == 0);
  };
  
  template <typename _Key> struct hash;
  
  template <> struct hash<Rational> {
  public:
    size_t operator() (const Rational& a) const {
      // variant where only two longs are considered:
      return mpz_getlimbn(a.get_num().get_mpz_t(), 0) ^ mpz_getlimbn(a.get_den().get_mpz_t(), 0);

      // the following can be used in case to many conflicts occur:
      size_t result = 0;
      for (mp_size_t i = 0; i < mpz_size(a.get_num().get_mpz_t()); ++i) {
	result ^= mpz_getlimbn(a.get_num().get_mpz_t(), i);
      }
      for (mp_size_t i = 0; i < mpz_size(a.get_den().get_mpz_t()); ++i) {
	result ^= mpz_getlimbn(a.get_den().get_mpz_t(), i);
      }
      return result;
    }
  };

};

#endif // RATIONAL_H

// Local Variables:
// mode:C++
// End:
