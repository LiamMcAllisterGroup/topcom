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

void StairCaseMatrix::_eliminate(const size_type& ridx, const size_type& cidx, const size_type& cno) {
  const size_type n = rowdim();
  
  if (is_zero((*this)(ridx,ridx))) {
#ifdef SUPER_VERBOSE
    std::cerr << "eraser = " << (*this)(ridx,ridx) << " == ZERO -> try colperm; row : " << ridx << ", col: " << ridx << std::endl;
#endif
    for (size_type k = cidx; k < cidx + cno; ++k) {
      if (!is_zero((*this)(ridx,k))) {
	(*this).swap_cols(ridx,k);
	_coefficient *= MINUSONE;
#ifdef SUPER_VERBOSE
	std::cerr << "eraser = " << (*this)(ridx,ridx) << " == ZERO -> colperm successful; row : " << ridx << ", col: " << ridx << std::endl;
#endif
	return;
      }
    }
    if (is_zero((*this)(ridx,ridx))) {
#ifdef SUPER_VERBOSE
      std::cerr << "eraser = " << (*this)(ridx,ridx) << " == ZERO -> colperm unsuccessful; row : " << ridx << ", col: " << ridx << std::endl;
#endif
      return;
    }
#ifdef SUPER_VERBOSE
    else {
      std::cerr << "eraser = " << (*this)(ridx,ridx) << " != ZERO; row : " << ridx << ", col: " << ridx << std::endl;
    }
#endif
  }
#ifdef SUPER_VERBOSE
  else {
    std::cerr << "eraser = " << (*this)(ridx,ridx) << " != ZERO; row : " << ridx << ", col: " << ridx << std::endl;
  }
#endif
  const Field& eraser = (*this)(ridx,ridx);
#ifdef SUPER_VERBOSE
  std::cerr << "eraser = " << eraser << "; row : " << ridx << ", col: " << ridx << std::endl;
#endif
  for (size_type j = cidx; j < cidx + cno; ++j) {
    const Field& delinquent = (*this)(ridx,j);
#ifdef SUPER_VERBOSE
    std::cerr << "delinquent = " << delinquent << "; row : " << ridx << ", col: " << j << std::endl;
#endif
    if (is_zero(delinquent)) {
      continue;
    }
    for (size_type k = ridx + 1; k < n; ++k) {
      (*this)(k,j) -= (*this)(k,ridx) * delinquent / eraser;
    }
    (*this)(ridx,j) = ZERO;
  }
#ifdef SUPER_VERBOSE
  std::cerr << "after step " << ridx << " of stair case transformation: " << std::endl;
  (*this).pretty_print(std::cerr);
#endif  
}

StairCaseMatrix& StairCaseMatrix::augment(const Matrix& matrix) {
  if (matrix.coldim() == 0) {
    return *this;
  }
#ifdef INDEX_CHECK
  assert(rowdim() == matrix.rowdim());
  assert(coldim() + matrix.coldim() <= rowdim());
#endif
  const size_type m = coldim();
  Matrix::augment(matrix);
#ifdef SUPER_VERBOSE
  std::cerr << "before stair case transformation:" << std::endl;
  pretty_print(std::cerr);
#endif
  for (size_type i = 0; i < m; ++i) {
    _eliminate(i, m, matrix.coldim());
  }
#ifdef SUPER_VERBOSE
  std::cerr << "after stair case transformation: " << std::endl;
  (*this).pretty_print(std::cerr);
#endif
  return *this;
}

const Field det(const StairCaseMatrix& matrix) {
#ifdef INDEX_CHECK
  assert(matrix.coldim() == matrix.rowdim());
#endif
  Field result(ONE);
  for (size_type i = 0; i < matrix.coldim(); ++i) {
    result *= matrix(i,i);
    if (is_zero(result)) {
      return result;
    }
  }
  return result * matrix._coefficient;
}

std::ostream& StairCaseMatrix::pretty_print(std::ostream& ost) {
  ost << "matrix: " << '\n';
  Matrix::pretty_print(ost);
  ost << "coefficient: " << _coefficient << '\n';
  return ost;
}
 
std::ostream& operator<<(std::ostream& ost, const StairCaseMatrix& matrix) {
  ost << (Matrix)matrix << '/' << matrix._coefficient;
  return ost;
}

// eof StairCaseMatrix.cc
