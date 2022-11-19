////////////////////////////////////////////////////////////////////////////////
// 
// LPinterface.hh 
//
//    produced: 2001/10/21 jr
// last change: 2020/04/02 jr
//
////////////////////////////////////////////////////////////////////////////////
#ifndef LPINTERFACE_HH
#define LPINTERFACE_HH

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <mutex>

#include "Vector.hh"
#include "Matrix.hh"
#include "LabelSet.hh"
#include "Rational.h"

#include "setoper.h"
#include "cdd.h"

namespace topcom {

  // we can use three variants of arithmetics in cddlib:

  // the following preprocessor magic is necessary because the word `Rational'
  // is used in an enum type in the cddlib:
  
#if defined GMPRATIONAL // GMP Rational
  
  inline void dd_set_R(mytype& a, const Rational& b) {
    mpq_set(a, b.get_mpq_t());
  }

#endif

  class LPinterface {
  private:
    dd_ErrorType      _err;
    dd_LPSolverType   _solver;
    dd_LPPtr          _lpptr;
    dd_LPSolutionPtr  _solptr; 
    dd_rowrange       _m;
    dd_colrange       _n;
    dd_MatrixPtr      _matrixptr;
    const LabelSet    _support;
    static bool       _is_initialized;
    static std::mutex _init_mutex;
  private:
    LPinterface();
  public:
    // constructors:
    LPinterface(const Matrix&, const LabelSet&);
    LPinterface(const LPinterface&) = delete;
    const LPinterface& operator=(const LPinterface&) = delete;

    // destructor:
    inline ~LPinterface();

    // cdd must be initialized and terminated,
    // which is best done only once:
    inline static void init();
    inline static void term();

    // functions:
    bool has_interior_point(Vector* heightsptr);
  };

  inline LPinterface::~LPinterface() {

    // Free allocated spaces:
    if (_solptr) {
      dd_FreeLPSolution(_solptr);
    }
    dd_FreeLPData    (_lpptr);
    dd_FreeMatrix    (_matrixptr);
  }

  // cdd must be initialized and terminated,
  // which is best done only once:
  inline void LPinterface::init() {
    std::lock_guard<std::mutex> init_lock(_init_mutex);
    if (_is_initialized) {
      return;
    }
    dd_set_global_constants();
    _is_initialized = true;
  }

  inline void LPinterface::term() {
    std::lock_guard<std::mutex> init_lock(_init_mutex);
    if (!_is_initialized) {
      return;
    }
    dd_free_global_constants();
    _is_initialized = false;
  }

}; // namespace topcom

#endif

// eof LPinterface.hh
