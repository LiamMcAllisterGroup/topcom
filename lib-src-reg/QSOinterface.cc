////////////////////////////////////////////////////////////////////////////////
// 
// QSOinterface.cc
//
//    produced: 2003/01/15 jr
// last change: 2003/01/15 jr
//
////////////////////////////////////////////////////////////////////////////////

#include <string>
#include <iostream>
#include <sstream>

#include "QSOinterface.hh"

namespace topcom {

  bool        QSOinterface::_is_initialized = false;
  std::mutex  QSOinterface::_init_mutex;

  // constructors:

  QSOinterface::QSOinterface(const Matrix& m, const LabelSet& support) :
    _lp(0),
    _support(support) {
    if (!_is_initialized) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "QSOinterface::QSOinterface(const Matrix& m, const LabelSet& support):"
		<< " QSopt_ex was called but not initialized - exiting" << std::endl;
      exit(1);
    }

    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "building QSopt_ex LP for matrix" << std::endl;
      m.pretty_print(std::cerr);
      std::cerr << " ..." << std::endl;
    }

    // the coefficient matrix m is transposed for efficiency reasons:
    _cmatrowdim = m.coldim();
    _cmatcoldim = m.rowdim();

    // store for each non-zero column the index of the original column:
    _nzc_of_col = (int*)malloc(_cmatcoldim * sizeof(int));
    _col_of_nzc = (int*)malloc(_cmatcoldim * sizeof(int));

    _cntnze = 0;
    _cntnzc = 0;
    for (int j = 0; j < _cmatcoldim; ++j) {
      bool is_nzc(false);
      for (int i = 0; i < _cmatrowdim; ++i) {
	if (m[i][j] != FieldConstants::ZERO) {
	  // entry in column i and row j of m become row i and column j in LP:
	  ++_cntnze;
	  is_nzc = true;
	}
      }
      if (is_nzc) {
	// save the column index bijection and inverse original column index <-> non-zero column index:
	_col_of_nzc[_cntnzc] = j;
	_nzc_of_col[j]       = _cntnzc;      
	++_cntnzc;
      }
    }
  
    // allocate memory for the sparse matrix type used by QSopt_ex:
    _cmatcnt  = (int*)  malloc(_cntnzc     * sizeof(int)); // no of nze in each column
    _cmatbeg  = (int*)  malloc(_cntnzc     * sizeof(int)); // start idx of each column
    _cmatind  = (int*)  malloc(_cntnze     * sizeof(int)); // indices of nze in each column
    _sense    = (char*) malloc(_cmatrowdim * sizeof(char)); // binary operator of constraints in each row
    _colnames = (char**)malloc(_cntnzc     * sizeof(char*)); // name for each column
    _rownames = (char**)malloc(_cmatrowdim * sizeof(char*)); // name for each row
    _cmatval  = (mpq_t*)malloc(_cntnze     * sizeof(mpq_t)); // value of each nze
    _obj      = (mpq_t*)malloc(_cntnzc     * sizeof(mpq_t)); // obj of each columns
    _rhs      = (mpq_t*)malloc(_cmatrowdim * sizeof(mpq_t)); // rhs of each row
    _lower    = (mpq_t*)malloc(_cntnzc     * sizeof(mpq_t)); // lb for each column
    _upper    = (mpq_t*)malloc(_cntnzc     * sizeof(mpq_t)); // ub for each column

    // allocate memory for the solution data:
  
    if ((_cmatcnt == 0)
	|| (_cmatbeg == 0)
	|| (_cmatind == 0)
	|| (_sense == 0)
	|| (_colnames == 0)
	|| (_rownames == 0)
	|| (_cmatval == 0)
	|| (_obj == 0)
	|| (_rhs == 0)
	|| (_lower == 0)
	|| (_upper == 0)) {
      std::cerr << "QSOinterface::QSOinterface(const Matrix& m, const LabelSet& support):"
		<< " memory allocation failed - exiting" << std::endl;
      exit(1);
    }

    // populate the sparse matrix -
    // note that the coefficient matrix m is transposed for efficiency reasons:
    int nextnze(0);
    int nextcol(0);
    for (size_type k = 0; k < _cntnzc; ++k) {
      int mcolnzecnt(0);
      _cmatbeg[k] = nextnze;
      for (size_type i = 0; i < _cmatrowdim; ++i) {
      
	// first, the entries of the coefficient matrix of the LP have to be added;
	// again, we have to use the coefficient matrix m in a transposed way
	// and shift the column index because m may contain zero rows;
	// since qsopt-ex uses sparse structures, we just enter non zeroes:
	if (m[i][_col_of_nzc[k]] != FieldConstants::ZERO) {
	  if (nextnze >= _cntnze) {
	    std::cerr << "index nextnze = " << nextnze
		      << " out of range " << _cntnze << " - exiting" << std::endl;
	    exit(1);
	  }
	  _cmatind[nextnze] = i;
	  mpq_init(_cmatval[nextnze]);
	  mpq_set(_cmatval[nextnze], m[i][_col_of_nzc[k]].get_mpq_t());
	  ++nextnze;
	  ++mcolnzecnt;
	}
      }
      _cmatcnt[k] = mcolnzecnt;
      mpq_init(_obj[k]);
      mpq_set(_obj[k], FieldConstants::ZERO.get_mpq_t());
      mpq_init(_lower[k]);
      mpq_set(_lower[k], mpq_ILL_MINDOUBLE);
      mpq_init(_upper[k]);
      mpq_set(_upper[k], mpq_ILL_MAXDOUBLE);
      std::stringstream ss;
      ss << "x" << _col_of_nzc[k];
      _colnames[k] = (char*)malloc((ss.str().length() + 1) * sizeof(char));
      strcpy(_colnames[k], ss.str().c_str());
    }
    for (size_type i = 0; i < _cmatrowdim; ++i) {
      _sense[i] = 'G';
      mpq_init(_rhs[i]);
      mpq_set(_rhs[i], FieldConstants::ONE.get_mpq_t());
      std::stringstream ss;
      ss << "c" << i;
      _rownames[i] = (char*)malloc((ss.str().length() + 1) * sizeof(char));    
      strcpy(_rownames[i], ss.str().c_str());
    }
    if (CommandlineOptions::debug()) {
      // write down the C-arrays in a readable from to
      // check the input:
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "raw LP data:" << std::endl;
      for (int k = 0; k < _cntnzc; ++k) {
	std::cerr << "col " << _colnames[k] << ": ";
	for (int e = 0; e < _cmatcnt[k]; ++e) {
	  int nzeidx = e + _cmatbeg[k];
	  std::cerr << _rownames[_cmatind[nzeidx]] << "->" << Rational(_cmatval[e + _cmatbeg[k]]) << " | ";
	}
	std::cerr << "(obj: " << _obj[k] << ", "
		  << _lower[k] << " <= " << _colnames[k] << " <= " << _upper[k]
		  << ")" << '\n';
      }
      std::cerr << "end LP data." << std::endl;
    }
    // next, we must construct a complete LP from
    // the coefficient matrix, the right hand side, and the sense;
    // the right hand side is zero but we need strict feasibility,
    // i.e., we seek an interior point of a homogeneous cone Ax > 0;
    // because of homogenity, we can safely set the right hand side to one, 
    // since every solution x with Ax > 0 can be scaled to a solution y with Ay >= 1;
    // moreover, the changing directions Ax <= -1 to Ax >= 1 in all inequalities at the same time
    // leads to an equivalent problem because if y is feasible to Ax <= -1 then
    // -y is feasible to Ax >= 1:
    _lp = mpq_QSload_prob("RegularityCheck",
			  _cntnzc,
			  _cmatrowdim,
			  _cmatcnt,
			  _cmatbeg,
			  _cmatind,
			  _cmatval,
			  QS_MAX,
			  _obj,
			  _rhs,
			  _sense,
			  _lower,
			  _upper,
			  (const char**)_colnames,
			  (const char**)_rownames);
  
    if (_lp == NULL) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "QSOinterface::QSOinterface(const Matrix& m):"
		<< " error in loading problem - exiting" << std::endl;
      exit(1);
    }
  
    int ncols = mpq_QSget_colcount(_lp);
    if (ncols != _cntnzc) {
      std::cerr << "wrong coldim in problem - exiting" << std::endl;
      exit(1);
    }
    int nrows = mpq_QSget_rowcount(_lp);
    if (nrows != _cmatrowdim) {
      std::cerr << "wrong rowdim in problem - exiting" << std::endl;
      exit(1);
    }
    _xsol = (mpq_t*)malloc(_cntnzc * sizeof(mpq_t));
    for (size_type j = 0; j < _cntnzc; ++j) {
      mpq_init(_xsol[j]);
    }
    _ysol = (mpq_t*)malloc(_cmatrowdim * sizeof(mpq_t));
    for (size_type i = 0; i < _cmatrowdim; ++i) {
      mpq_init(_ysol[i]);
    }
    _basis = (QSbasis*)malloc(sizeof(QSbasis));
    memset (_basis, 0, sizeof (QSbasis));
  
    if (CommandlineOptions::verbose()) {
      // print LP to file:
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      mpq_QSwrite_prob(_lp, "RegularityCheck.lp", "LP");
    }
  }

  // destructor:
  QSOinterface::~QSOinterface() {
    if (_nzc_of_col) {
      free(_nzc_of_col);
    }
    if (_col_of_nzc) {
      free(_col_of_nzc);
    }	
    if (_lp) {
      mpq_QSfree_prob(_lp);
    }
    // free temporary data:
    for (size_type k = 0; k < _cntnzc; ++k) {
      mpq_clear(_xsol[k]);
    }
    free(_xsol);
    for (size_type i = 0; i < _cmatrowdim; ++i) {
      mpq_clear(_ysol[i]);
    }
    free(_ysol);
    free(_basis);
    for (int e = 0; e < _cntnze; ++e) {
      mpq_clear(_cmatval[e]);
    }
    free(_cmatval);
    for (int k = 0; k < _cntnzc; ++k) {
      mpq_clear(_obj[k]);
      mpq_clear(_lower[k]);
      mpq_clear(_upper[k]);
    }
    free(_obj);
    free(_upper);
    free(_lower);
    for (int i = 0; i < _cmatrowdim; ++i) {
      mpq_clear(_rhs[i]);
    }
    free(_rhs);
    for (size_type k = 0; k < _cntnzc; ++k) {
      free(_colnames[k]);
    }  
    free(_colnames);
    for (size_type i = 0; i < _cmatrowdim; ++i) {
      free(_rownames[i]);
    }
    free(_rownames);
    free(_sense);
    free(_cmatind);
    free(_cmatbeg);
    free(_cmatcnt);
    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "... destroying QSopt_ex LP done." << std::endl;
    }
  }

  // functions:
  bool QSOinterface::has_interior_point(Vector* heightsptr) {
    if (!_is_initialized) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "QSOinterface::has_interior_point():"
		<< " QSopt_ex was called but not initialized - exiting" << std::endl;
      exit(1);
    }
    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "solving QSopt_ex LP for strict feasibility ..." << std::endl;
    }
  
    bool result(false);
    int status(0);
    int rval(0);
  
    if (!CommandlineOptions::debug()) {
      FILE* resultptr = freopen("/dev/null", "w", stderr);
    }
    rval = QSexact_solver(_lp, nullptr, nullptr, nullptr, DUAL_SIMPLEX, &status);
    if (!CommandlineOptions::debug()) {
      FILE* resultptr = freopen("/dev/tty", "w", stderr);
    }
    if (rval != 0) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      mpq_QSwrite_prob(_lp, "RegularityCheck_with_QSopt_ex_error.lp", "LP");
      std::cerr << "QSOinterface::has_interior_point():"
		<< " QSopt_ex could not solve the LP - exiting" << std::endl;
      exit(1);
    }
    if (status == QS_LP_OPTIMAL) {
      if (CommandlineOptions::output_heights()) {
	rval = mpq_QSget_x_array(_lp, _xsol);
	if (rval != 0) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  mpq_QSwrite_prob(_lp, "RegularityCheck_with_QSopt_ex_error.lp", "LP");
	  std::cerr << "QSOinterface::has_interior_point():"
		    << " QSopt_ex could not get the solution vector - exiting" << std::endl;
	  exit(1);
	}
      
	// compute a sufficiently large height for unused points:
	Field maxheight(FieldConstants::ONE);
	for (int k = 0; k < _cntnzc; ++k) {
	  if (maxheight - FieldConstants::ONE < Field(_xsol[k])) {
	    maxheight = Field(_xsol[k]) + FieldConstants::ONE;
	  }
	}
	for (int j = 0; j < _cmatcoldim; ++j) {
	  if (_support.contains(j)) {
	    heightsptr->at(j) = Field(_xsol[_nzc_of_col[j]]);
	  }
	  else {	  
	    heightsptr->at(j) = maxheight;
	  }
	}
      }
      result = true;
    }
    else {
      result = false;
    }
    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "... solving QSopt_ex LP done." << std::endl;
    }  
    return result;
  }

}; // namespace topcom

// eof QSOinterface.cc
