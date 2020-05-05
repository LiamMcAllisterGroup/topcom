////////////////////////////////////////////////////////////////////////////////
// 
// Integer.h
//
//    produced: 2009/04/30 jr
// last change: 2009/04/30 jr
// 
////////////////////////////////////////////////////////////////////////////////
#ifndef _INTEGER_H
#define _INTEGER_H

#include "gmpxx.h"

typedef mpz_class Integer;

namespace std_ext {
   template <typename _Key> struct hash;

   template <> struct hash<Integer> {
   public:
      size_t operator() (const Integer& a) const {
	size_t result = 0;
	for (mp_size_t i = 0; i < mpz_size(a.get_mpz_t()); ++i) {
	  result ^= mpz_getlimbn(a.get_mpz_t(), i);
	}
	return result;
      }
   };
}

#endif // INTEGER_H

// Local Variables:
// mode:C++
// End:
