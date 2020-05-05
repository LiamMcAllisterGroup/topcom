////////////////////////////////////////////////////////////////////////////////
// 
// LPinterface.hh 
//
//    produced: 2001/10/21 jr
// last change: 2001/10/21 jr
//
////////////////////////////////////////////////////////////////////////////////
#ifndef LPINTERFACE_HH
#define LPINTERFACE_HH

#include "setoper.h"
#include "Rational.h"

// the following preprocessor magic is necessary because the word `Rational'
// is used in an enum type in the cddlib:

#include "cdd.h"

// we can use three variants of arithmetics in cddlib:

#if defined GMPRATIONAL // GMP Rational
  
inline void dd_set_R(mytype& a, const Rational& b) {
  //  mpq_init(a);
  mpq_set(a, b.get_mpq_t());
}

#elif defined GMPFLOAT // GMP float
  
inline void dd_set_R(mytype& a, const Rational& b) {
  //  mpq_init(a);
  mpq_set(a, double(b));
}

#else // built-in C double 
  
inline void dd_set_R(mytype& a, const Rational& b) {
  a[0] = double(b);
}

#endif

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

#include "Matrix.hh"
#include "IntegerSet.hh"

class LPinterface {
private:
  dd_ErrorType     _err;
  dd_LPSolverType  _solver;
  dd_LPPtr         _lpptr;
  dd_LPSolutionPtr _solptr; 
  dd_rowrange      _m;
  dd_colrange      _n;
  dd_MatrixPtr     _matrixptr;
  const IntegerSet _support;
private:
  LPinterface();
public:
  // constructors:
  LPinterface(const Matrix&, const IntegerSet&);
  // destructor:
  inline ~LPinterface();
  // functions:
  bool has_interior_point();
};

inline LPinterface::~LPinterface() {
  
  // Free allocated spaces:
  dd_FreeLPSolution(_solptr);
  dd_FreeLPData    (_lpptr);
  dd_FreeMatrix    (_matrixptr);
  dd_free_global_constants();
}

#endif

// eof LPinterface.hh
