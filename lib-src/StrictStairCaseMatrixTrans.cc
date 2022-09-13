////////////////////////////////////////////////////////////////////////////////
// 
// StrictStairCaseMatrixTrans.cc
//
//    produced: 30/01/2020 jr
// last change: 20/01/2020 jr
// 
////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <ctype.h>
#include <string.h>

#include "Vector.hh"
#include "StrictStairCaseMatrixTrans.hh"

namespace topcom {

  // we eliminate the columns with indices cidx to cidx + cno (exclusive) as follows
  // with transformation information:
  // ------------------------------------------------------------------------------
  //        | already eliminated part | uneliminated part
  //        | 0 1 .. ridx      cidx-1 | cidx ... cidx+cno
  // ----------------------------------------------------------------------------------
  //        | * 0 ... 0 ......... 0   | 0  0 ...  0 
  //        | * * 0 . . ......... .   | .  .      . 
  //        | * * * 0 . ......... .   | .  .      . 
  //        | * * * * 0 ......... .   | 0  0      0 
  //   rdix | * * * *(*)0 ....... .   |(*)(*)    (*)
  //        | * * * * * * 0 ..... .   | *  *      * 
  //        | * * * * * * * 0 ... 0   | *  * ...  *
  // ------------------------------------------------------------------------------
  //      0 | the columns in this part contain the coefficients
  //        | of the representation of the column above as
  //        | a linear combination of original columns;
  // transf | used mainly for the computation of circuit signatures:
  //        | below the left-most zero column above 
  //        | we find the dependency in this transformation column,
  //        | and its signs are a circuit signature;
  // cidx-1 | initially allocated with rowdim+1 rows, enlarged if needed

  void StrictStairCaseMatrixTrans::_eliminate(const parameter_type ridx,
					      const parameter_type cidx,
					      const parameter_type cno) {
#ifdef SUPER_VERBOSE
    std::cerr << "called elimination to eliminate the elements (" << ridx << ", " << cidx << ")"
	      << " through (" << ridx << ", " << cidx + cno - 1 << ")"
	      << " ..." << std::endl;
    std::cerr << "starting elimination with start pivot (" << ridx << ", " << ridx << ") out of "
	      << rowdim() << ":" << std::endl;
#endif
    const parameter_type rdim = rowdim();
    const parameter_type cdim = coldim();
    parameter_type rowidx = ridx;
    while (rowidx < rdim) {
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
	    _transformation.swap_cols(ridx,k);
#ifdef SUPER_VERBOSE
	    std::cerr << "eraser = " << (*this)(rowidx,ridx) << " != FieldConstants::ZERO -> colperm successful; row : " << rowidx << ", col: " << ridx << std::endl;
	    pretty_print(std::cerr);
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
#ifdef SUPER_VERBOSE
      std::cerr << "elimination of column " << j << " (" << j - cidx + 1 << " of " << cno << ") many columns to eliminate:" << std::endl;
#endif
      const Field& delinquent = (*this)(rowidx,j); // do not copy; careful: delinquent changes with that matrix element!
#ifdef SUPER_VERBOSE
      std::cerr << "delinquent = " << delinquent << "; row : " << rowidx << ", col: " << j << std::endl;
#endif
      if (is_zero(delinquent)) {
	continue;
      }
      const Field multiplier = delinquent / eraser;
      for (parameter_type k = 0; k < cdim; ++k) {
	_transformation(k,j) -= multiplier * _transformation(k,ridx);
      }
      for (parameter_type k = rowidx + 1; k < rdim; ++k) {
	(*this)(k,j) -=  multiplier * (*this)(k,ridx);
      }
      (*this)(rowidx,j) = FieldConstants::ZERO;
    }
#ifdef SUPER_VERBOSE
    std::cerr << "after step " << ridx << " of stair case transformation: " << std::endl;
    (*this).pretty_print(std::cerr);
#endif  
  }

  StrictStairCaseMatrixTrans& StrictStairCaseMatrixTrans::augment(const Vector& vector) {
#ifdef INDEX_CHECK
    assert(rowdim() == vector.dim());
#endif
    if (empty()) {

      // if the matrix has no columns yet,
      // we can assign the StrictStairCaseMatrix
      // constructed from a single vector:
      return operator=(StrictStairCaseMatrixTrans(vector));
    }
    const parameter_type cdim(coldim());
    const parameter_type rdim(rowdim());
    const parameter_type m = cdim < rdim ? cdim : rdim;
    Matrix::augment(vector);
    _transformation.augment(Vector(vector.dim() + 1, FieldConstants::ZERO));
    if (_transformation.rowdim() < cdim + 1) {
      for (parameter_type col = 0; col < _transformation.coldim(); ++col) {
	_transformation.col(col).resize(cdim + 1);
      }
    }
    _transformation(cdim, cdim) = FieldConstants::ONE;
#ifdef SUPER_VERBOSE
    std::cerr << "before strict stair case transformation:" << std::endl;
    pretty_print(std::cerr);
#endif
    for (parameter_type i = 0; i < m; ++i) {
      this->_eliminate(i, cdim, 1);
    }
#ifdef SUPER_VERBOSE
    std::cerr << "after strict stair case transformation: " << std::endl;
    (*this).pretty_print(std::cerr);
#endif
    return *this;
  }

  StrictStairCaseMatrixTrans& StrictStairCaseMatrixTrans::augment(const Matrix& matrix) {
#ifdef INDEX_CHECK
    assert(rowdim() == matrix.rowdim());
#endif
    for (parameter_type col = 0; col < matrix.coldim(); ++col) {
      augment(matrix.col(col));
    }
    return *this;
  }

  std::ostream& StrictStairCaseMatrixTrans::pretty_print(std::ostream& ost) const {
    ost << "matrix: " << '\n';
    Matrix::pretty_print(ost);
    ost << "coefficient: " << _coefficient << '\n';
    ost << "transformation:" << '\n';
    _transformation.pretty_print(ost);
    ost << '\n';
    return ost;
  }
 
  std::ostream& operator<<(std::ostream& ost, const StrictStairCaseMatrixTrans& matrix) {
    ost << (Matrix)matrix << '/' << matrix._coefficient;
    return ost;
  }

}; // namespace topcom

// eof StrictStairCaseMatrixTrans.cc
