////////////////////////////////////////////////////////////////////////////////
// 
// LPinterface.cc
//
//    produced: 2001/10/28 jr
// last change: 2001/10/28 jr
//
////////////////////////////////////////////////////////////////////////////////

#include <mutex>

#include "LPinterface.hh"

namespace topcom {

  bool        LPinterface::_is_initialized = false;
  std::mutex  LPinterface::_init_mutex;

  // constructors:

  LPinterface::LPinterface(const Matrix& m, const LabelSet& support) :
    _solptr(0),
    _support(support) {
    if (!_is_initialized) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "LPinterface::LPinterface(const Matrix& m, const LabelSet& support):"
		<< " Lp solver cdd was called but not initialized - exiting" << std::endl;
      exit(1);
    }

    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "building LP matrix ..." << std::endl;
    }
    _solver                     = dd_DualSimplex;
  
    // note that the coefficient matrix m is transposed for efficiency reasons;
    _m                          = m.coldim(); // number of rows
    _n                          = m.rowdim() + 1; // number of cols
    _matrixptr                  = dd_CreateMatrix(_m, _n);
    _matrixptr->objective       = dd_LPmax;
    _matrixptr->representation  = dd_Inequality;
    _matrixptr->numbtype        = dd_Rational;

    for (size_type i = 0; i < m.coldim(); ++i) {

      // build cdd's matrix row by row;
      // recall that the coefficient matrix m is transposed for efficiency reasons;
      // the first entry is MINUSONE in every row because
      // its meaning in cdd is is "minus the right hand side":
      dd_set_R(_matrixptr->matrix[i][0], FieldConstants::MINUSONE);
      for (size_type j = 0; j < m.rowdim(); ++j) {
      
	// because the first column contains the data for the right hand side 
	// (which is MINUSONE) we need to shift the column index by one;
	// again, we have to use the coefficient matrix m in a transposed way:
	dd_set_R(_matrixptr->matrix[i][j+1], m(j, i));
      }
    }
    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      dd_WriteMatrix(stderr, _matrixptr);
    }

    // cdd allows to automatically generate an auxilliary LP for finding an interior point:
    // dd_LPPtr auxlpptr = dd_Matrix2LP(_matrixptr, &_err);
    // _lpptr = dd_MakeLPforInteriorFinding(auxlpptr);
    _lpptr = dd_Matrix2Feasibility(_matrixptr, &_err);
    if (!_lpptr) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "error in generating LP." << std::endl;
      dd_WriteErrorMessages(stderr, _err);
      exit(1);
    }
    // dd_FreeLPData(auxlpptr);
    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "... done." << std::endl;
    }
  }

  // functions:
  bool LPinterface::has_interior_point(Vector* heightsptr) {

    // caller must take care of a pointer to a Vector of appropriate size, pre-filled with ZERO:
    if (!_is_initialized) {
      std::cerr << "LPinterface::has_interior_point():"
		<< " LP solver cdd was called but not initialized - exiting" << std::endl;
      exit(1);
    }

    // Check feasibility with cdd LP library:
    bool result = dd_LPSolve0(_lpptr, _solver, &_err);
    if (_err != dd_NoError) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "error in solving LP." << std::endl;
      dd_WriteErrorMessages(stderr, _err);
      exit(1);
    }

    if (_lpptr->LPS == dd_Inconsistent) {

      // the LP is infeasible:
      if (CommandlineOptions::debug()) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "The feasible region is empty." << std::endl;
      }
      if (CommandlineOptions::debug()) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "... done." << std::endl;
      }
      return false;
    }
  
    // otherwise the LP is feasible:
    if (CommandlineOptions::output_heights()) {
      // output a height vector:
      _solptr = dd_CopyLPSolution(_lpptr);
      Field maxheight(FieldConstants::ONE);
      for (dd_rowrange j = 0; j < (_solptr->d) - 1; j++) {
	const Field x_j = Field(_solptr->sol[j+1]);
	if (maxheight - FieldConstants::ONE < x_j) {
	  maxheight = x_j + FieldConstants::ONE;
	}
      }
      for (dd_rowrange j = 0; j < (_solptr->d) - 1; j++) {
	const Field x_j = Field(_solptr->sol[j+1]);
	if (_support.contains(j)) {
	  heightsptr->at(j) = x_j;
	  if (CommandlineOptions::debug()) {
	    std::cerr << "-- point " << j << " used, assigning height " << x_j << " --" << std::endl;
	  }
	}
	else {
	  if (CommandlineOptions::debug()) {
	    std::cerr << "-- point " << j << " unused, assigning height " << maxheight << " --" << std::endl;
	  }
	  heightsptr->at(j) = maxheight;
	}
      }
      if (CommandlineOptions::debug()) {
	std::cerr << "used points: " << _support << std::endl;
	std::cerr << "optimal slack: ";
	dd_WriteNumber(stderr, _solptr->optvalue);
	std::cerr << std::endl;
      }
    }
    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "... done." << std::endl;
    }
    return true;
  }

}; // namespace topcom

// eof LPinterface.cc
