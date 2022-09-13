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
#include <iostream>

#include "Matrix.hh"

namespace topcom {

  const char Matrix::col_delim_char = '\t';

  const char Matrix::start_char = '[';
  const char Matrix::end_char   = ']';
  const char Matrix::delim_char = ',';

  const parameter_type Matrix::col_capacity = 0;

  const Vector Matrix::row(const parameter_type index) const {
    Vector result(coldim());
    for (parameter_type i = 0; i < coldim(); ++i) {
      result[i] = (*this)(index, i);
    }
    return result;
  }
  void Matrix::row_resize(const parameter_type new_memsize, const Field& init_entry) {
    for (parameter_type i = 0; i < coldim(); ++i) {
      this->col(i).resize(new_memsize,init_entry);
    }
  }
  Matrix& Matrix::canonicalize() {
    for (parameter_type i = 0; i < coldim(); ++i) {
      this->col(i).canonicalize();
    }
    return *this;  
  }

  Matrix& Matrix::add(const Matrix& matrix) {
#ifdef INDEX_CHECK
    assert((rowdim() == matrix.rowdim()) && (coldim() == matrix.coldim()));
#endif
    for (parameter_type i = 0; i < coldim(); ++i) {
      this->col(i).add(matrix.col(i));
    }
    return *this;
  }
  Matrix& Matrix::scale(const Field& entry) {
    for (parameter_type i = 0; i < coldim(); ++i) {
      this->col(i).scale(entry);
    }
    return *this;
  }

  Matrix& Matrix::augment(const Matrix& matrix, const IntegerSet& ignored_cols) {
#ifdef INDEX_CHECK
    if (coldim() > 0) {
      assert(rowdim() == matrix.rowdim());
    }
#endif
#ifndef STL_CONTAINERS  
    push_back(matrix);
#else
    // reserve(coldim() + matrix.coldim());
    for (parameter_type col = 0; col < matrix.coldim(); ++col) {
      if (ignored_cols.contains(col)) {
	continue;
      }
      push_back(matrix.col(col));
    }
#endif
    return *this;
  }

  Matrix& Matrix::stack(const Matrix& matrix) {
#ifdef INDEX_CHECK
    assert(coldim() == matrix.coldim());
#endif
    for (parameter_type i = 0; i < coldim(); ++i) {
      this->col(i).stack(matrix.col(i));
    }
    return *this;
  }

  Matrix& Matrix::swap_cols(const parameter_type i, const parameter_type j) {
#ifdef INDEX_CHECK
    assert((i < coldim()) && (j < coldim()));
#endif
    this->col(i).swap(this->col(j));
    // const Vector tmp = (*this)[i];
    // (*this)[i] = (*this)[j];
    // (*this)[j] = tmp;
    return *this;
  }
  
  Matrix& Matrix::swap_rows(const parameter_type i, const parameter_type j) {
#ifdef INDEX_CHECK
    assert((i < rowdim()) && (j < rowdim()));
#endif
    for (parameter_type k = 0; k < coldim(); ++k) {
      std::swap((*this)(i, k), (*this)(j, k));
    }
    return *this;
  }
  
  Matrix& Matrix::row_echelon_form(const parameter_type start_row, const parameter_type start_col, const Field& scale) {
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
    parameter_type minnonzerocol(coldim());
    parameter_type minnonzerocolrow(rowdim());
    Field new_scale(scale);
    for (parameter_type i = start_row; i < rowdim(); ++i) {
      for (parameter_type j = start_col; j < coldim(); ++j) {
	if (((*this)(i, j) != FieldConstants::ZERO) && (j < minnonzerocol)) {
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
      new_scale = new_scale * FieldConstants::MINUSONE;
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
    for (parameter_type i = start_row + 1; i < rowdim(); ++i) {
      if ((*this)(i, minnonzerocol) != FieldConstants::ZERO) {
#ifdef SUPER_VERBOSE
	if (CommandlineOptions::verbose()) {
	  std::cerr << "\t eliminating row " << i << " ..." << std::endl;
	}
#endif
	const Field delinquent((*this)(i, minnonzerocol));
	const Field multiplier = delinquent / eraser;

	for (parameter_type j = minnonzerocol; j < coldim(); ++j) {
	  (*this)(i,j) -= multiplier * (*this)(start_row,j);
	}
#ifdef SUPER_VERBOSE
	if (CommandlineOptions::verbose()) {
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
    return row_echelon_form(start_row + 1, minnonzerocol + 1, new_scale);
  }

  const Field Matrix::left_upper_det() const {
    parameter_type n = coldim();
    if (coldim() > rowdim()) {
      n = rowdim();
    }
    Matrix tmp(*this);
    tmp.resize(n);
    Field scale(FieldConstants::ONE);
    for (parameter_type i = 0; i < n - 1; ++i) {
      if (tmp(i,i) == FieldConstants::ZERO) {
	for (parameter_type k = i + 1; k < n; ++k) {
	  if (tmp(i,k) != FieldConstants::ZERO) {
	    tmp.swap_cols(i,k);
	    scale *= -1;
	    continue;
	  }
	}
	if (tmp(i,i) == FieldConstants::ZERO) {
	  return FieldConstants::ZERO;
	}
      }
      Field eraser = tmp(i,i);
#ifdef SUPER_VERBOSE
      std::cerr << "eraser = " << eraser << std::endl;
#endif
      for (parameter_type j = i + 1; j < n; ++j) {
	Field delinquent = tmp(i,j);
	if (delinquent == FieldConstants::ZERO) {
	  continue;
	}
#ifdef SUPER_VERBOSE
	std::cerr << "delinquent = " << delinquent << std::endl;
#endif
	for (parameter_type k = i + 1; k < n; ++k) {
	  tmp(k,j) -= (tmp(k,i) * delinquent / eraser);
	}
	tmp(i,j) = FieldConstants::ZERO;
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
    Field result(FieldConstants::ONE);
    for (parameter_type i = 0; i < n; ++i) {
      result *= tmp(i,i);
    }
#ifdef SUPER_VERBOSE
    std::cerr << "Triangular determinant is " << result << std::endl;
#endif
    return result * scale;
  }

  Matrix Matrix::transpose() const {
    Matrix result(coldim(), rowdim());
    for (parameter_type i = 0; i < coldim(); ++i) {
      for (parameter_type j = 0; j < rowdim(); ++j) {
	result(i, j) = (*this)(j, i);
      }
    }
    return result;
  }
  
  const Matrix multiply(const Matrix& matrix1, const Matrix& matrix2) {
#ifdef INDEX_CHECK
    assert(matrix1.coldim() == matrix2.rowdim());
#endif
    Matrix result(matrix1.rowdim(), matrix2.coldim());
    for (parameter_type i = 0; i < matrix1.rowdim(); ++i) {
      const Vector row = matrix1.row(i);
      for (parameter_type j = 0; j < matrix2.coldim(); ++j) {
	result(i, j) = inner_product(row, matrix2.col(j));
      }
    }
    return result;
  }

  // transformation:
  Vector Matrix::StackOfAllColumns() const {
    Vector result;
    for (parameter_type j = 0; j < this->coldim(); ++j) {
      result.stack(this->col(j));
    }
    return result;
  }

  std::ostream& Matrix::pretty_print(std::ostream& ost) const {
    for (parameter_type i = 0; i < rowdim(); ++i) {
      const Vector row_vector = row(i);
      for (parameter_type j = 0; j < coldim(); ++j) {
	ost << row_vector(j) << col_delim_char;
      }
      ost << std::endl;
    }
    return ost;
  }

  std::istream& Matrix::read(std::istream& ist) {
    if (CommandlineOptions::debug()) {
      std::cerr << "Matrix::read(std::istream& ist): "
		<< "current matrix: " << *this << std::endl;
    }
  
    char c;
    Vector column;

    matrix_data::clear();
    ist >> std::ws >> c;
    if (c == start_char) {
      while (ist >> std::ws >> c) {
	if (c == end_char) {
	  break;
	}
	if (c == delim_char) {
	  continue;
	}
	ist.putback(c);
	if (ist >> column) {
	  matrix_data::push_back(column);	
	}
	else {
	  if (CommandlineOptions::debug()) {
	    std::cerr << "Matrix::read(std::istream& ist):"
		      << c << " not of appropriate type." << std::endl;
	  }
	  ist.clear(std::ios::failbit);
	  return ist;
	}
      }
    }
    else {
      if (CommandlineOptions::debug()) {
	std::cerr << "Matrix::read(std::istream& ist):"
		  << " missing `" << start_char << "'." << std::endl;
      }
      ist.clear(std::ios::failbit);
      return ist;
    }
    ist.clear(std::ios::goodbit);
    return ist;
  }

  std::ostream& Matrix::write(std::ostream& ost) const {
    ost << start_char;
    if (coldim() > 0) {
      for (parameter_type i = 0; i < coldim() - 1; ++i) {
	ost << (*this)[i] << delim_char;
      }
      if (coldim() > 0) {
	ost << (*this)[coldim() - 1];
      }
    }
    ost << end_char;
    return ost;
  }

}; // namespace topcom

// eof Matrix.cc
