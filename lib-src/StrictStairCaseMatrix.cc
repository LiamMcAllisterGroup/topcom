////////////////////////////////////////////////////////////////////////////////
// 
// StrictStairCaseMatrix.cc
//
//    produced: 30/01/2020 jr
// last change: 30/01/2020 jr
// 
////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <ctype.h>
#include <string.h>

#include "StrictStairCaseMatrix.hh"

namespace topcom {

  // we eliminate the columns with indices cidx to cidx + cno (exclusive) as follows:
  // ------------------------------------------------------------------------------
  // already eliminated part | uneliminated part
  // 0 1 .. ridx      cidx-1 | cidx ... cidx+cno-1
  // ------------------------------------------------------------------------------
  // * 0 ... 0 ......... 0   | 0  0 ...  0 
  // * * 0 . . ......... .   | .  .      . 
  // * * * 0 . ......... .   | .  .      . 
  // * * * * 0 ......... .   | 0  0      0 
  // * * * *(*)0 ....... .   |(*)(*)    (*)
  // * * * * * * 0 ..... .   | *  *      * 
  // * * * * * * * 0 ... .   | *  * ...  * 

  void StrictStairCaseMatrix::_eliminate(const parameter_type ridx,
					 const parameter_type cidx,
					 const parameter_type cno) {
#ifdef SUPER_VERBOSE
    std::cerr << "starting elimination of columns "
	      << "[" << cidx << ", " << cidx + cno << ")"
	      << " with start pivot (" << ridx << ", " << ridx << ") out of "
	      << rowdim() << ":" << std::endl;
#endif
    const parameter_type n = rowdim();
    parameter_type rowidx = ridx;
    while (rowidx < n) {
#ifdef SUPER_VERBOSE
      std::cerr << "current pivot (" << rowidx << ", " << ridx << ")" << std::endl;
#endif
      if (is_zero((*this)(rowidx,ridx))) {
#ifdef SUPER_VERBOSE
	std::cerr << "eraser = " << (*this)(rowidx,ridx) << " == FieldConstants::ZERO -> try colperm; row : " << rowidx << ", col: " << ridx << std::endl;
#endif
	for (parameter_type k = cidx; k < cidx + cno; ++k) {
	  if (!is_zero((*this)(rowidx,k))) {
	    Matrix::swap_cols(ridx,k);
	    _coefficient *= FieldConstants::MINUSONE;
#ifdef SUPER_VERBOSE
	    std::cerr << "eraser = " << (*this)(rowidx,ridx) << " != FieldConstants::ZERO -> colperm successful; row : " << rowidx << ", col: " << ridx << std::endl;
#endif
	    return;
	  }
	}
	if (is_zero((*this)(rowidx,ridx))) {
#ifdef SUPER_VERBOSE
	  std::cerr << "eraser = " << (*this)(rowidx,ridx) << " == FieldConstants::ZERO -> colperm unsuccessful; row : " << rowidx << ", col: " << ridx << std::endl;
#endif
	}
	else {
#ifdef SUPER_VERBOSE
	  std::cerr << "eraser = " << (*this)(rowidx,ridx) << " != FieldConstants::ZERO; row : " << rowidx << ", col: " << ridx << std::endl;
#endif
	  break;
	}
      }
      else {
#ifdef SUPER_VERBOSE
	std::cerr << "eraser = " << (*this)(rowidx,ridx) << " != FieldConstants::ZERO; row : " << rowidx << ", col: " << ridx << std::endl;
#endif
	break;
      }
      ++rowidx;
    }
    if (rowidx == rowdim()) {
#ifdef SUPER_VERBOSE
      std::cerr << "all potential erasers == FieldConstants::ZERO for all possible swaps of columns: zero columns; row : all, col: " << ridx << std::endl;
#endif
      return;
    }
    const Field& eraser = (*this)(rowidx,ridx); // do not copy; careful: eraser changes with that matrix element!
#ifdef SUPER_VERBOSE
    std::cerr << "eraser = " << eraser << "; row : " << rowidx << ", col: " << ridx << std::endl;
#endif
    for (parameter_type j = cidx; j < cidx + cno; ++j) {
      const Field& delinquent = (*this)(rowidx,j); // do not copy; careful: delinquent changes with that matrix element!
#ifdef SUPER_VERBOSE
      std::cerr << "delinquent = " << delinquent << "; row : " << rowidx << ", col: " << j << std::endl;
#endif
      if (is_zero(delinquent)) {
	continue;
      }
      const Field multiplier = delinquent / eraser;
      for (parameter_type k = rowidx + 1; k < n; ++k) {
	(*this)(k,j) -=  multiplier * (*this)(k,ridx);
      }
      (*this)(rowidx,j) = FieldConstants::ZERO;
    }
#ifdef SUPER_VERBOSE
    std::cerr << "after step " << ridx << " of stair case transformation: " << std::endl;
    (*this).pretty_print(std::cerr);
#endif  
  }

  StrictStairCaseMatrix& StrictStairCaseMatrix::augment(const Matrix& matrix) {
    if (matrix.coldim() == 0) {
      return *this;
    }
#ifdef INDEX_CHECK
    assert(rowdim() == matrix.rowdim());
    assert(coldim() + matrix.coldim() <= rowdim() + 1);
#endif
    const parameter_type first_uneliminated_col(coldim());
    
    // eliminate only up to lower triangular form:
    parameter_type max_elim_rows = first_uneliminated_col;
    if (max_elim_rows > rowdim()) {
      
      // more than rowdim() many rows cannot be eliminated:
      max_elim_rows = rowdim();
    }
    Matrix::augment(matrix);
#ifdef SUPER_VERBOSE
    std::cerr << "before strict stair case transformation:" << std::endl;
    pretty_print(std::cerr);
#endif
    for (parameter_type i = 0; i < max_elim_rows; ++i) {
      this->_eliminate(i, first_uneliminated_col, matrix.coldim());
    }
#ifdef SUPER_VERBOSE
    std::cerr << "after strict stair case transformation: " << std::endl;
    (*this).pretty_print(std::cerr);
#endif
    return *this;
  }

  // determinant form induced by the first rowdim - 1 columns:
  const Field StrictStairCaseMatrix::valuation(const Vector& vector) const {
    StrictStairCaseMatrix tmp(*this, 0, rowdim() - 1);
    tmp.augment(vector);
    return tmp.left_upper_det();
  }

  std::ostream& StrictStairCaseMatrix::pretty_print(std::ostream& ost) const {
    ost << "matrix: " << '\n';
    Matrix::pretty_print(ost);
    ost << "coefficient: " << _coefficient << '\n';
    return ost;
  }
 
  std::ostream& operator<<(std::ostream& ost, const StrictStairCaseMatrix& matrix) {
    ost << (Matrix)matrix << '/' << matrix._coefficient;
    return ost;
  }

}; // namespace topcom

// eof StrictStairCaseMatrix.cc
