////////////////////////////////////////////////////////////////////////////////
// 
// Field.hh 
//
//    produced: 21/08/97 jr
// last change: 30/10/97 jr
//
////////////////////////////////////////////////////////////////////////////////
#ifndef FIELD_HH
#define FIELD_HH

#include <Rational.h>
#include "Global.hh"

namespace topcom {

  typedef Rational Field;

  class FieldConstants {
  public:
    static const Field ZERO;
    static const Field ONE;
    static const Field MINUSONE;
  };
  
  inline const short int sign(const Field& f) { 
    if (f == FieldConstants::ZERO) { 
      return 0; 
    }
    else if (f > FieldConstants::ZERO) {
      return 1;
    }
    else {
      return -1;
    }
  }
  
  inline Integer binomial(const Integer& n, const Integer& k) {
    Integer kk(k);
    if (n - k < k) {
      kk = n - k;
    }
    if (k == 0) {
      return 1;
    }
    if (k == 1) {
      return n;
    }
    if (kk < 1) {
      return 1;
    }
    Integer result(1);
    for (Integer i = 0; i < kk; ++i) {
      result *= n - i;
      result /= i + 1;
    }
    return result;
  }

}; // namespace topcom

#endif

// eof Field.hh
