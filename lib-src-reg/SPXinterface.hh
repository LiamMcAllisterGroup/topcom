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

#include "Vector.hh"
#include "Matrix.hh"

// Soplex includes (make only sense if soplex has been installed with GMP and BOOST):
#include "soplex.h"

namespace topcom {

  inline soplex::Rational __field_to_soplexrational(const Field& q) {
    return soplex::Rational(*(mpq_t*)q.get_mpq_t());
  }
  inline Field __soplexrational_to_field(const soplex::Rational& q) {
    return Field(q.backend().data());
  }
};

namespace topcom {
  
  class SPXinterface {
  private:
    soplex::SoPlexBase<soplex::Real> _soplex_obj;
    const LabelSet                   _support;
  private:
    inline SPXinterface() {}
  public:
    // constructors:
    SPXinterface(const Matrix&, const LabelSet&);

    // destructor:
    inline ~SPXinterface();

    // soplex need not be initialized and terminated,
    // so this is for consistency only:
    inline static void init() {}
    inline static void term() {}

    // functions:
    bool has_interior_point(Vector* heightsptr);

  };

  inline SPXinterface::~SPXinterface() {}

}; // namespace topcom

#endif // HAVE_LIBSOPLEX

#endif

// eof SPX interface.hh
