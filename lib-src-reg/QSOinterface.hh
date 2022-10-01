////////////////////////////////////////////////////////////////////////////////
// 
// QSOinterface.hh 
//
//    produced: 2020/04/01 jr
// last change: 2020/04/01 jr
//
////////////////////////////////////////////////////////////////////////////////
#ifndef QSOINTERFACE_HH
#define QSOINTERFACE_HH

#ifdef HAVE_LIBQSOPTEX

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

#include "Vector.hh"
#include "Matrix.hh"
#include "LabelSet.hh"

// QSOPT_ex includes:
extern "C" {
#include "qsopt_ex/QSopt_ex.h"
}

namespace topcom {

  class QSOinterface {
  private:
    // tmp data for the problem description:
    int                _cmatrowdim;
    int                _cmatcoldim;
    int                _cntnze;
    int                _cntnzc;
    int*               _cmatcnt;
    int*               _cmatbeg;
    int*               _cmatind;
    char*              _sense;
    char**             _colnames; 
    char**             _rownames;
    mpq_t*             _cmatval;
    mpq_t*             _obj;
    mpq_t*             _rhs;   
    mpq_t*             _lower;   
    mpq_t*             _upper;
    mpq_QSprob         _lp;
    // tmp data for the results:
    mpq_t*             _ysol;
    mpq_t*             _xsol;
    QSbasis*           _basis;
    // input data needed for the output of heights for *each* point:
    int*               _col_of_nzc;
    int*               _nzc_of_col;
    const LabelSet     _support;
    // sync data:
    static bool        _is_initialized;
    static std::mutex  _init_mutex;
  private:
    QSOinterface() {}
    QSOinterface(const QSOinterface&) {}
  public:
    // constructors:
    QSOinterface(const Matrix&, const LabelSet&);

    // destructor:
    ~QSOinterface();

    // QSopt must be initialized and terminated,
    // which is best done only once:
    inline static void init();
    inline static void term();
  
    // functions:
    bool has_interior_point(Vector* heightsptr);
  };

  // QSopt must be initialized and terminated,
  // which is best done only once:
  inline void QSOinterface::init() {
    if (_is_initialized) {
      return;
    }
    if (CommandlineOptions::verbose()) {
      QSopt_ex_version();
    }
    std::lock_guard<std::mutex> init_lock(_init_mutex);
    QSexactStart();
    QSexact_set_precision (128);
    _is_initialized = true;
  }

  inline void QSOinterface::term() {
    if (!_is_initialized) {
      return;
    }
    std::lock_guard<std::mutex> init_lock(_init_mutex);
    // currently the clearance seg-faults -- deactivated:
    // QSexactClear();
    _is_initialized = false;
  }

}; // namespace topcom

#endif // HAVE_LIBQSOPTEX

#endif

// eof QSOinterface.hh
