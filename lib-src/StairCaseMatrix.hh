////////////////////////////////////////////////////////////////////////////////
// 
// StairCaseMatrix.hh 
//
//    produced: 21/08/97 jr
// last change: 30/10/97 jr
//
////////////////////////////////////////////////////////////////////////////////
#ifndef STAIRCASEMATRIX_HH
#define STAIRCASEMATRIX_HH

#include <assert.h>

#include "CommandlineOptions.hh"

#include "Vector.hh"
#include "Matrix.hh"

class StairCaseMatrix : public Matrix {
private:
  Field _coefficient;
public:
  // constructors:
  inline StairCaseMatrix();
  inline StairCaseMatrix(const StairCaseMatrix&);
  inline StairCaseMatrix(const Vector&);
  inline StairCaseMatrix(const size_type&, 
			 const size_type&, 
			 const Field& init_entry = ZERO);
  // accessors:
  inline const Field& coefficient() const;
  // in place operations:
  StairCaseMatrix& augment(const Matrix&);
  // out of place operations:
  inline bool has_full_rank() const;
  friend const Field det(const StairCaseMatrix&);
  // iostream:
  std::ostream& pretty_print(std::ostream& ost);
  friend std::ostream& operator<<(std::ostream& ost, const StairCaseMatrix& matrix);
private:
  // internal elimination step:
  void _eliminate(const size_type&, const size_type&, const size_type&);
  // forbid in place operations if result is of type StairCaseMatrix:
  inline StairCaseMatrix& add(const StairCaseMatrix&);
  inline StairCaseMatrix& scale(const Field&);
  inline StairCaseMatrix& stack(const StairCaseMatrix&);
  inline StairCaseMatrix& swap_cols(const size_type, const size_type);
  inline StairCaseMatrix& swap_rows(const size_type, const size_type);
};

inline StairCaseMatrix::StairCaseMatrix() : 
  Matrix(), _coefficient(ONE) {
}
inline StairCaseMatrix::StairCaseMatrix(const StairCaseMatrix& matrix) : 
  Matrix(matrix), _coefficient(matrix._coefficient) {
}
inline StairCaseMatrix::StairCaseMatrix(const Vector& vector) : 
  Matrix(vector), _coefficient(ONE) {
}
inline StairCaseMatrix::StairCaseMatrix(const size_type& init_rows, 
					const size_type& init_cols, 
					const Field& init_entry) :
  Matrix(init_rows, init_cols) , _coefficient(ONE) {
}
inline const Field& StairCaseMatrix::coefficient() const {
  return _coefficient;
}
inline bool StairCaseMatrix::has_full_rank() const {
  if (coldim() == 0) {
    return true;
  }
  else {
    return (!(*this)[coldim()-1].is_zero());
  }
}

// private in place operations (senseless in general; only for internal use):
inline StairCaseMatrix& StairCaseMatrix::add(const StairCaseMatrix& matrix) {
  Matrix::add(matrix);
  return *this;
}

inline StairCaseMatrix& StairCaseMatrix::scale(const Field& scalar) {
  Matrix::scale(scalar);
  return *this;
}

inline StairCaseMatrix& StairCaseMatrix::stack(const StairCaseMatrix& matrix) {
  Matrix::stack(matrix);
  return *this;
}

inline StairCaseMatrix& StairCaseMatrix::swap_cols(const size_type j1, const size_type j2) {
  Matrix::swap_cols(j1, j2);
  return *this;
}

inline StairCaseMatrix& StairCaseMatrix::swap_rows(const size_type i1, const size_type i2) {
  Matrix::swap_rows(i1, i2);
  return *this;
}

#endif

// eof StairCaseMatrix.hh
