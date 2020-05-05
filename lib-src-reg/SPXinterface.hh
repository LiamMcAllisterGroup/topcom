////////////////////////////////////////////////////////////////////////////////
// 
// SPXinterface.hh 
//
//    produced: 2003/01/05 jr
// last change: 2003/01/05 jr
//
////////////////////////////////////////////////////////////////////////////////
#ifndef SPXINTERFACE_HH
#define SPXINTERFACE_HH

#ifdef HAVE_LIBSOPLEX

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

#include "Matrix.hh"

// Soplex includes:
#include "spxdefines.h"

#include "spxdefaultpr.h"
#include "spxdefaultrt.h"
#include "slufactor.h"
#include "spxequilisc.h"
#include "spxaggregatesm.h"

#include "svector.h"
#include "dsvector.h"
#include "lprow.h"
#include "spxlp.h"
#include "soplex.h"

class SPXinterface {
private:
  soplex::SoPlex          _soplex_obj;
  soplex::SPxLP           _lp_obj;
  soplex::SPxDefaultPR*   _spxpricer_ptr;
  soplex::SPxDefaultRT*   _spxtester_ptr;
  soplex::SLUFactor*      _spxsolver_ptr;
  soplex::SPxEquili*      _spxscaler_ptr;
  soplex::SPxAggregateSM* _spxsimplifier_ptr;
private:
  inline SPXinterface() {}
public:
  // constructors:
  SPXinterface(const Matrix&);

  // destructor:
  inline ~SPXinterface();

  // functions:
  bool has_interior_point();
};

inline SPXinterface::~SPXinterface() {
  if (_spxpricer_ptr) {
    delete _spxpricer_ptr;
  }
  if (_spxtester_ptr) {
    delete _spxtester_ptr;
  }
  if (_spxsolver_ptr) {
    delete _spxsolver_ptr;
  }
  if (_spxscaler_ptr) {
    delete _spxscaler_ptr;
  }
  if (_spxsimplifier_ptr) {
    delete _spxsimplifier_ptr;
  }
}

#endif // HAVE_LIBSOPLEX

#endif

// eof SPX interface.hh
