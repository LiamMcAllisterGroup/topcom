////////////////////////////////////////////////////////////////////////////////
// 
// LPinterface.cc
//
//    produced: 2001/10/28 jr
// last change: 2001/10/28 jr
//
////////////////////////////////////////////////////////////////////////////////

#include "LPinterface.hh"

// constructors:

LPinterface::LPinterface(const Matrix& m, const IntegerSet& support) :
  _support(support) {
  if (CommandlineOptions::debug()) {
    std::cerr << "building LP matrix ..." << std::endl;
  }

  dd_set_global_constants();
  _solver                     = dd_DualSimplex;
  
  // note that the coefficient matrix m is transposed for efficiency reasons;
  _m                          = m.coldim(); // number of rows
  _n                          = m.rowdim() + 1; // number of cols
  _matrixptr                  = dd_CreateMatrix(_m, _n);
  _matrixptr->objective       = dd_LPmax;
  _matrixptr->numbtype        = dd_Rational;

  for (size_type i = 0; i < m.coldim(); ++i) {

    // build cdd's matrix row by row;
    // recall that the coefficient matrix m is transposed for efficiency reasons;
    // the first entry is ZERO in every row because
    // its meaning in cdd is is "minus the right hand side":
    dd_set_R(_matrixptr->matrix[i][0], ZERO);
    for (size_type j = 0; j < m.rowdim(); ++j) {

      // because the first column contains the data for the right hand side 
      // (which is ZERO) we need to shift the column index by one;
      // again, we have to use the coefficient matrix m in a transposed way:
      dd_set_R(_matrixptr->matrix[i][j+1], m(j, i));
    }
  }
  if (CommandlineOptions::debug()) {
    dd_WriteMatrix(stderr, _matrixptr);
  }

  // cdd allows to automatically generate an auxilliary LP for finding an interior point:
  dd_LPPtr auxlpptr = dd_Matrix2LP(_matrixptr, &_err);
  _lpptr = dd_MakeLPforInteriorFinding(auxlpptr);
  if (!_lpptr) {
    std::cerr << "error in generating LP." << std::endl;
    dd_WriteErrorMessages(stderr, _err);
    exit(1);
  }
  dd_FreeLPData(auxlpptr);
  if (CommandlineOptions::debug()) {
    std::cerr << "... done." << std::endl;
  }
}

// functions:
bool LPinterface::has_interior_point() {
  
  static long idx(0);

  // Check feasibility with cdd LP library:
  const bool result = dd_LPSolve(_lpptr, _solver, &_err);
  if (_err != dd_NoError) {
    std::cerr << "error in solving LP." << std::endl;
    dd_WriteErrorMessages(stderr, _err);
    exit(1);
  }
  
  // Write an interior point:
  _solptr = dd_CopyLPSolution(_lpptr);
  if (dd_Positive(_solptr->optvalue)){
    if (CommandlineOptions::output_heights()) {
      std::cout << "(";
      Field maxheight(ONE);
      for (dd_rowrange j = 0; j < (_solptr->d) - 2; j++) {
	const Field x_j = Field(_solptr->sol[j+1]);
	if (maxheight - 1 < x_j) {
	  maxheight = x_j + 1;
	}
      }
      for (dd_rowrange j = 0; j < (_solptr->d) - 3; j++) {
	const Field x_j = Field(_solptr->sol[j+1]);
	if (_support.contains(j)) {
	  //	  std::cout << double(x_j);
	  std::cout << x_j.get_str();
	}
	else {
	  if (CommandlineOptions::debug()) {
	    std::cerr << "point " << j << " unused, assigning height " << maxheight + 1 << std::endl;
	  }
	  //	  std::cout << double(maxheight) + 1;
	  std::cout << maxheight.get_str();
	}
	std::cout << ",";
      }
      const long j((_solptr->d) - 3);
      const Field x_j = Field(_solptr->sol[j+1]);
      if (_support.contains(j)) {
	//	std::cout << double(x_j);
	std::cout << x_j.get_str();
      }
      else {
	if (CommandlineOptions::debug()) {
	  std::cerr << "point " << j << " unused, assigning height " << maxheight + 1 << std::endl;
	}
	//	std::cout << double(maxheight) + 1;
	std::cout << maxheight.get_str();
      }
      std::cout << ")" << std::endl;
      if (CommandlineOptions::debug()) {
	std::cerr << "Used points: " << _support << std::endl;
	std::cerr << "Optimal slack: ";
	dd_WriteNumber(stderr, _solptr->optvalue);
	std::cerr << std::endl;
      }
    }
    if (CommandlineOptions::debug()) {
      std::cerr << "... done." << std::endl;
    }
    return true;
  }
  if (dd_Negative(_solptr->optvalue)) {
    if (CommandlineOptions::debug()) {
      std::cerr << "The feasible region is empty." << std::endl;
    }
    if (CommandlineOptions::debug()) {
      std::cerr << "... done." << std::endl;
    }
    return false;
  }
  if (dd_EqualToZero(_solptr->optvalue)) {
    if (CommandlineOptions::debug()) {
      std::cerr << "... done." << std::endl;
    }
    if (CommandlineOptions::debug()) {
      std::cerr << "The feasible region is nonempty but has no interior point." << std::endl;
    }
    return false;
  }
  if (CommandlineOptions::debug()) {
    std::cerr << "WARNING: unspecified case. I assume that no interior point exists." << std::endl;
  }
  return false;
}

// eof LPinterface.cc
