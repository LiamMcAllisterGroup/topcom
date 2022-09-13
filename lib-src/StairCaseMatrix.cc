////////////////////////////////////////////////////////////////////////////////
// 
// StairCaseMatrix.cc
//
//    produced: 13/03/98 jr
// last change: 13/03/98 jr
// 
////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <ctype.h>
#include <string.h>

#include "StairCaseMatrix.hh"

namespace topcom {

  void StairCaseMatrix::_eliminate(const parameter_type ridx, const parameter_type cidx, const parameter_type cno) {
    if (cno == 0) {
      return;
    }
    const parameter_type n = rowdim();
  
    if (is_zero((*this)(ridx,ridx))) {
#ifdef SUPER_VERBOSE
      std::cerr << "eraser = " << (*this)(ridx,ridx) << " == FieldConstants::ZERO -> try colperm; row : " << ridx << ", col: " << ridx << std::endl;
#endif
      for (parameter_type k = cidx; k < cidx + cno; ++k) {
	if (!is_zero((*this)(ridx,k))) {
	  Matrix::swap_cols(ridx,k);
	  _coefficient *= FieldConstants::MINUSONE;
#ifdef SUPER_VERBOSE
	  std::cerr << "eraser = " << (*this)(ridx,ridx) << " == FieldConstants::ZERO -> colperm successful; row : " << ridx << ", col: " << ridx << std::endl;
#endif
	  break;
	}
      }
      if (is_zero((*this)(ridx,ridx))) {
#ifdef SUPER_VERBOSE
	std::cerr << "eraser = " << (*this)(ridx,ridx) << " == FieldConstants::ZERO -> colperm unsuccessful; row : " << ridx << ", col: " << ridx << std::endl;
#endif
	return;
      }
#ifdef SUPER_VERBOSE
      else {
	std::cerr << "eraser = " << (*this)(ridx,ridx) << " != FieldConstants::ZERO; row : " << ridx << ", col: " << ridx << std::endl;
      }
#endif
    }
#ifdef SUPER_VERBOSE
    else {
      std::cerr << "eraser = " << (*this)(ridx,ridx) << " != FieldConstants::ZERO; row : " << ridx << ", col: " << ridx << std::endl;
    }
#endif
    const Field& eraser = (*this)(ridx,ridx);
#ifdef SUPER_VERBOSE
    std::cerr << "eraser = " << eraser << "; row : " << ridx << ", col: " << ridx << std::endl;
#endif
    for (parameter_type j = cidx; j < cidx + cno; ++j) {
      const Field& delinquent = (*this)(ridx,j);
#ifdef SUPER_VERBOSE
      std::cerr << "delinquent = " << delinquent << "; row : " << ridx << ", col: " << j << std::endl;
#endif
      if (is_zero(delinquent)) {
	continue;
      }
      // An alternative division-free elimination step (slightly slower for large numbers):
      // if (eraser < FieldConstants::ZERO) {
      //   _coefficient /= -eraser;
      //   for (parameter_type k = ridx + 1; k < n; ++k) {
      // 	(*this)(k,j) *= -eraser;
      // 	(*this)(k,j) += delinquent * (*this)(k,ridx);
      //   }
      // }
      // else {
      //   _coefficient /= eraser;
      //   for (parameter_type k = ridx + 1; k < n; ++k) {
      // 	(*this)(k,j) *= eraser;
      // 	(*this)(k,j) -= delinquent * (*this)(k,ridx);
      //   }
      // }
      const Field multiplier = delinquent / eraser;
      for (parameter_type k = ridx + 1; k < n; ++k) {
	(*this)(k,j) -= multiplier * (*this)(k,ridx);
      }
      (*this)(ridx,j) = FieldConstants::ZERO;
    }
#ifdef SUPER_VERBOSE
    std::cerr << "after step " << ridx << " of stair case transformation: " << std::endl;
    (*this).pretty_print(std::cerr);
#endif  
  }

  StairCaseMatrix& StairCaseMatrix::augment(const Matrix& matrix, const IntegerSet& ignored_cols) {

    // the resulting matrix will have matrix.rowdim() rows;
    // note that (*this) may be empty resulting in rowdim() = 0:
    if (matrix.coldim() == 0) {
      return *this;
    }
    if (empty()) {
      parameter_type j = 0;
      while(j < matrix.coldim()) {
	if (!ignored_cols.contains(j)) {
	  this->push_back(matrix.col(j));
	  return this->augment(matrix, ignored_cols + j);
	}
	else {
	  ++j;
	}
      }
      return *this;
    }
#ifdef INDEX_CHECK
    if (coldim() > 0) {
      assert(rowdim() == matrix.rowdim());
    }
#endif
    const parameter_type m = matrix.rowdim();

    // so many columns are already in stair-case form:
    const parameter_type n = coldim();

    // augment the matrix without elimination:
    Matrix::augment(matrix, ignored_cols);

    // so many columns must be in stair-case form after the transformations:
    const parameter_type n_new = coldim();

    // cnt1 many column elimination are needed with old pivot columns:
    const parameter_type cnt1 = std::min<parameter_type>(n, m);

    // cnt2 - cnt1 many column eliminations are needed with already modified new columns:
    const parameter_type cnt2 = std::min<parameter_type>(n_new, m);
  
    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "before stair case transformation:" << std::endl;
      pretty_print(std::cerr);
    }
  
    // eliminate new columns using all old columns:
    for (parameter_type i = 0; i < cnt1; ++i) {

      // use pivot (i, i) to eliminate (n_new - n) many columns starting at column n:
      _eliminate(i, n, n_new - n);
      if (CommandlineOptions::debug()) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "after elimination with pivot " << i
		  << ", startcol " << n
		  << ", no " << n_new - n << ":" << std::endl;
	(*this).pretty_print(std::cerr);
      }
    }

    // eliminate new columns using new columns:
    for (parameter_type i = cnt1; i < cnt2; ++i) {

      // use pivot (i, i) to eliminate (n_new - i - 1) many columns starting at column i + 1:
      _eliminate(i, i + 1, n_new - i - 1);
      if (CommandlineOptions::debug()) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "after elimination with pivot " << i
		  << ", startcol " << i + 1
		  << ", no " << n_new - i - 1 << ":" << std::endl;
	(*this).pretty_print(std::cerr);
      }
    }
    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "after stair case transformation: " << std::endl;
      (*this).pretty_print(std::cerr);
    }
    return *this;
  }

  // this special form of "det" computes the determinant of the left-top-most
  // square matrix:
  const Field StairCaseMatrix::left_upper_det() const {
    parameter_type n = coldim();
    if (coldim() > rowdim()) {
      n = rowdim();
    }
    Field result(FieldConstants::ONE);
    for (parameter_type i = 0; i < n; ++i) {
      result *= (*this)(i,i);
      if (is_zero(result)) {
	return result;
      }
    }
    return result * _coefficient;
  }

  const Field StairCaseMatrix::det() const {
#ifdef INDEX_CHECK
    assert(coldim() == rowdim());
#endif
    return left_upper_det();
  }

  std::ostream& StairCaseMatrix::pretty_print(std::ostream& ost) const {
    ost << "matrix: " << '\n';
    Matrix::pretty_print(ost);
    ost << "coefficient: " << _coefficient << '\n';
    return ost;
  }
 
  std::ostream& operator<<(std::ostream& ost, const StairCaseMatrix& matrix) {
    ost << (Matrix)matrix << '/' << matrix._coefficient;
    return ost;
  }

}; // namespace topcom

// eof StairCaseMatrix.cc
