////////////////////////////////////////////////////////////////////////////////
// 
// Matrix.hh 
//
//    produced: 21/08/97 jr
// last change: 30/10/97 jr
//
////////////////////////////////////////////////////////////////////////////////
#ifndef MATRIX_HH
#define MATRIX_HH

#include <assert.h>
#include <iostream>

#include "Array.hh"

#include "CommandlineOptions.hh"

#include "Vector.hh"

typedef Array<Vector> matrix_data;

class Matrix : public matrix_data {
public:
  static const char col_delim_char;
public:
  // constructors:
  inline Matrix();
  inline Matrix(const Matrix&);
  inline Matrix(const Vector&);
  inline Matrix(const size_type, const size_type, const Field& init_entry = ZERO);
  // destructor:
  inline ~Matrix();
  // assignment:
  Matrix& operator=(const Matrix&);
  // accessors:
  inline const size_type rowdim() const;
  inline const size_type coldim() const;
  const        Vector row(const size_type) const;
  inline const Vector& col(const size_type) const;
  // resizing:
  void        row_resize(const size_type, const Field& init_entry = ZERO);
  inline void col_resize(const size_type, const Field& init_entry = ZERO);
  // indexing:
  inline       Field& operator()(const size_type, const size_type);
  inline const Field& operator()(const size_type, const size_type) const;
  // in place operations:
  Matrix& canonicalize();
  Matrix& add(const Matrix&);
  Matrix& scale(const Field&);
  Matrix& augment(const Matrix&);
  Matrix& stack(const Matrix&);
  Matrix& swap_cols(const size_type, const size_type);
  Matrix& swap_rows(const size_type, const size_type);
  Matrix& row_normal_form(const size_type start_row = 0, const size_type start_col = 0, const Field& scale = 1);
  // out of place operations:
  friend const Field left_upper_det(const Matrix&);
  inline friend const Field det(const Matrix&);
  friend const Matrix transpose(const Matrix&);
  friend const Matrix multiply(const Matrix&, const Matrix&);
  // transformation:
  Vector StackOfAllColumns() const;
  // iostream:
  std::ostream& pretty_print(std::ostream& ost) const;
  // overload of istream with canonicalize:
  inline std::istream& read(std::istream&);
  inline friend std::istream& operator>>(std::istream&, Matrix&);
};

inline Matrix::Matrix() : matrix_data() {
}
inline Matrix::Matrix(const Matrix& matrix) : matrix_data(matrix) {
}
inline Matrix::Matrix(const Vector& vector) : matrix_data(1,vector) {
}
inline Matrix::Matrix(const size_type init_rowdim, 
		      const size_type init_coldim, 
		      const Field& init_entry) : 
  matrix_data(init_coldim) {
  for (size_type i = 0; i < init_coldim; ++i) {
    (*this)[i] = Vector(init_rowdim, init_entry);
  }
}
inline Matrix::~Matrix() {}

inline const size_type Matrix::rowdim() const {
  assert(maxindex() != 0);
  return (*this)[0].maxindex();
}
inline const size_type Matrix::coldim() const {
  return maxindex();
}

inline const Vector& Matrix::col(const size_type index) const {
  return (*this)[index];
}

inline void Matrix::col_resize(const size_type new_size, const Field& init_entry) {
  matrix_data::resize(new_size, Vector(rowdim(), init_entry));
}

inline Field& Matrix::operator()(const size_type row, const size_type col) {
  return (*this)[col][row];
}

inline const Field& Matrix::operator()(const size_type row, const size_type col) const {
  return (*this)[col][row];
}

inline const Field det(const Matrix& matrix) {
#ifdef INDEX_CHECK
  assert(matrix.coldim() == matrix.rowdim());
#endif
  return left_upper_det(matrix);
}

inline std::istream& Matrix::read(std::istream& ist) {
  matrix_data::read(ist);
  this->canonicalize();
  return ist;
}

inline std::istream& operator>>(std::istream& ist, Matrix& m) {
  return m.read(ist);
}

#endif

// eof Matrix.hh
