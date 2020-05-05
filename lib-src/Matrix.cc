////////////////////////////////////////////////////////////////////////////////
// 
// Matrix.cc
//
//    produced: 13/03/98 jr
// last change: 13/03/98 jr
// 
////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <ctype.h>
#include <string.h>

#include "Matrix.hh"

const char Matrix::col_delim_char = '\t';

// assignment:
Matrix& Matrix::operator=(const Matrix& matrix) {
  if (this == &matrix) {
    return *this;
  }
  matrix_data::operator=(matrix);
  return *this;
}

const Vector Matrix::row(const size_type index) const {
  Vector result(coldim());
  for (size_type i = 0; i < coldim(); ++i) {
    result[i] = (*this)[i][index];
  }
  return result;
}
void Matrix::row_resize(const size_type new_size, const Field& init_entry) {
  for (size_type i = 0; i < coldim(); ++i) {
    (*this)[i].resize(new_size,init_entry);
  }
}
Matrix& Matrix::canonicalize() {
  for (size_type i = 0; i < coldim(); ++i) {
    (*this)[i].canonicalize();
  }
  return *this;  
}

Matrix& Matrix::add(const Matrix& matrix) {
#ifdef INDEX_CHECK
  assert((rowdim() == matrix.rowdim()) && (coldim() == matrix.coldim()));
#endif
  for (size_type i = 0; i < coldim(); ++i) {
    (*this)[i].add(matrix[i]);
  }
  return *this;
}
Matrix& Matrix::scale(const Field& entry) {
  for (size_type i = 0; i < coldim(); ++i) {
    (*this)[i].scale(entry);
  }
  return *this;
}

Matrix& Matrix::augment(const Matrix& matrix) {
#ifdef INDEX_CHECK
  if (coldim() > 0) {
    assert(rowdim() == matrix.rowdim());
  }
#endif
  append(matrix);
  return *this;
}
Matrix& Matrix::stack(const Matrix& matrix) {
#ifdef INDEX_CHECK
  assert(coldim() == matrix.coldim());
#endif
  for (size_type i = 0; i < coldim(); ++i) {
    (*this)[i].stack(matrix[i]);
  }
  return *this;
}

Matrix& Matrix::swap_cols(const size_type i, const size_type j) {
#ifdef INDEX_CHECK
  assert((i < coldim()) && (j < coldim()));
#endif
  const Vector tmp = (*this)[i];
  (*this)[i] = (*this)[j];
  (*this)[j] = tmp;
  return *this;
}
Matrix& Matrix::swap_rows(const size_type i, const size_type j) {
#ifdef INDEX_CHECK
  assert((i < rowdim()) && (j < rowdim()));
#endif
  for (size_type k = 0; k < coldim(); ++k) {
    Vector& col = (*this)[k];
    const Field tmp = col[i];
    col[i] = col[j];
    col[j] = tmp;
  }
  return *this;
}
Matrix& Matrix::row_normal_form(const size_type start_row, const size_type start_col, const Field& scale) {
#ifdef SUPER_VERBOSE
  if (CommandlineOptions::debug()) {
    std::cerr << "calling elimination of" << std::endl;
    this->pretty_print(std::cerr);
    std::cerr << "below row " << start_row << " to the right of column " << start_col << std::endl;
  }
#endif

  // swap row with left-most non-zero to the top at start_row:
#ifdef SUPER_VERBOSE
  if (CommandlineOptions::debug()) {
    std::cerr << "swapping left-most non-zero to the top ..." << std::endl;
  }
#endif
  if ((start_row == rowdim()) || (start_col == coldim())) {
    return *this;
  }
  size_type minnonzerocol(coldim());
  size_type minnonzerocolrow(rowdim());
  Field new_scale(scale);
  for (size_type i = start_row; i < rowdim(); ++i) {
    for (size_type j = start_col; j < coldim(); ++j) {
      if (((*this)(i, j) != ZERO) && (j < minnonzerocol)) {
	minnonzerocol = j;
	minnonzerocolrow = i;
      }
    }
  }
  if (minnonzerocolrow != start_row) {
#ifdef SUPER_VERBOSE
    if (CommandlineOptions::debug()) {
      std::cerr << "... by swapping row " << start_row << " and row " << minnonzerocolrow << " ..." << std::endl;
    }
#endif
    swap_rows(minnonzerocolrow, start_row);
    new_scale = new_scale * MINUSONE;
  }
#ifdef SUPER_VERBOSE
  if (CommandlineOptions::debug()) {
    this->pretty_print(std::cerr);
    std::cerr << "... done." << std::endl;
  }
#endif
  
  // eliminate under left-most non-zero:
#ifdef SUPER_VERBOSE
  if (CommandlineOptions::debug()) {
    std::cerr << "eliminating all non-zeros below element at index (" << start_row << "," << minnonzerocol << ") ..." << std::endl;
  }
#endif
  const Field eraser((*this)(start_row, minnonzerocol));
  for (size_type i = start_row + 1; i < rowdim(); ++i) {
    if ((*this)(i, minnonzerocol) != ZERO) {
#ifdef SUPER_VERBOSE
      if (CommandlineOptions::debug()) {
	std::cerr << "\t eliminating row " << i << " ..." << std::endl;
      }
#endif
      const Field delinquent((*this)(i, minnonzerocol));
      for (size_type j = minnonzerocol; j < coldim(); ++j) {
	(*this)(i,j) -= delinquent / eraser * (*this)(start_row,j);
      }
#ifdef SUPER_VERBOSE
      if (CommandlineOptions::debug()) {
	this->pretty_print(std::cerr);
	std::cerr << "\t ... done." << std::endl;
      }
#endif
    }
  }
#ifdef SUPER_VERBOSE
  if (CommandlineOptions::debug()) {
    this->pretty_print(std::cerr);
    std::cerr << "... done." << std::endl;
  }
#endif

  // delete trailing zero rows below start_row:
#ifdef SUPER_VERBOSE
  if (CommandlineOptions::debug()) {
    std::cerr << "deleting all-zero rows ..." << std::endl;
  }
#endif
  while (row(rowdim() - 1).is_zero()) {
#ifdef SUPER_VERBOSE
    if (CommandlineOptions::debug()) {
      std::cerr << "\t deleting row " << rowdim() - 1 << " ..." << std::endl;
    }
#endif
    row_resize(rowdim() - 1);
  }
#ifdef SUPER_VERBOSE
  if (CommandlineOptions::debug()) {
    this->pretty_print(std::cerr);
    std::cerr << "... done." << std::endl;
  }
#endif
  return row_normal_form(start_row + 1, minnonzerocol + 1, new_scale);
}

const Field left_upper_det(const Matrix& matrix) {
  size_type n = matrix.coldim();
  if (matrix.coldim() > matrix.rowdim())
    n = matrix.rowdim();

  Matrix tmp = matrix;
  tmp.resize(n);
  Field scale(ONE);
  for (size_type i = 0; i < n - 1; ++i) {
    if (tmp(i,i) == ZERO) {
      for (size_type k = i + 1; k < n; ++k) {
	if (tmp(i,k) != ZERO) {
	  tmp.swap_cols(i,k);
	  scale *= -1;
	  continue;
	}
      }
      if (tmp(i,i) == ZERO) {
	return ZERO;
      }
    }
    Field eraser = tmp(i,i);
#ifdef SUPER_VERBOSE
    std::cerr << "eraser = " << eraser << std::endl;
#endif
    for (size_type j = i + 1; j < n; ++j) {
      Field delinquent = tmp(i,j);
      if (delinquent == ZERO) {
	continue;
      }
#ifdef SUPER_VERBOSE
      std::cerr << "delinquent = " << delinquent << std::endl;
#endif
      for (size_type k = i + 1; k < n; ++k) {
	tmp(k,j) -= (tmp(k,i) * delinquent / eraser);
      }
      tmp(i,j) = ZERO;
    }
#ifdef SUPER_VERBOSE
    std::cerr << "current tmp = " << std::endl;
    tmp.pretty_print(std::cerr);
#endif
  }
#ifdef SUPER_VERBOSE
  std::cerr << "Triangular transformation is:" << std::endl;
  tmp.pretty_print(std::cerr);
  std::cerr << "Scale factor is " << scale << std::endl;
#endif
  Field result(ONE);
  for (size_type i = 0; i < n; ++i) {
    result *= tmp(i,i);
  }
#ifdef SUPER_VERBOSE
  std::cerr << "Triangular determinant is " << result << std::endl;
#endif
  return result * scale;
}

const Matrix transpose(const Matrix& matrix) {
  Matrix result(matrix.coldim(), matrix.rowdim());
  for (size_type i = 0; i < matrix.coldim(); ++i) {
    for (size_type j = 0; j < matrix.rowdim(); ++j) {
      result[j][i] = matrix[i][j];
    }
  }
  return result;
}
const Matrix multiply(const Matrix& matrix1, const Matrix& matrix2) {
#ifdef INDEX_CHECK
  assert(matrix1.coldim() == matrix2.rowdim());
#endif
  Matrix result(matrix1.rowdim(), matrix2.coldim());
  for (size_type i = 0; i < matrix1.rowdim(); ++i) {
    const Vector row = matrix1.row(i);
    for (size_type j = 0; j < matrix2.coldim(); ++j) {
      result[j][i] = inner_product(row, matrix2.col(j));
    }
  }
  return result;
}

// transformation:
Vector Matrix::StackOfAllColumns() const {
  Vector result;
  for (size_type j = 0; j < this->coldim(); ++j) {
    result.stack(this->col(j));
  }
  return result;
}

std::ostream& Matrix::pretty_print(std::ostream& ost) const {
  for (size_type i = 0; i < rowdim(); ++i) {
    const Vector row_vector = row(i);
    for (size_type j = 0; j < coldim(); ++j) {
      ost << row_vector[j] << col_delim_char;
    }
    ost << std::endl;
  }
  return ost;
}

// eof Matrix.cc
