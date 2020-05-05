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

typedef Rational Field;

static const Field ZERO     = Field(0);
static const Field ONE      = Field(1);
static const Field MINUSONE = Field(-1);

inline const int sign(const Field& f) { 
  if (f == ZERO) { 
    return 0; 
  }
  else if (f > ZERO) {
    return 1;
  }
  else {
    return -1;
  }
}

#endif

// eof Field.hh
