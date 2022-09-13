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
#include <limits>

#include "Permutation.hh"

#include "CommandlineOptions.hh"

#include "Vector.hh"
#include "IntegerSet.hh"
#include "IntegerSet64.hh"

#ifndef STL_CONTAINERS
#include "Array.hh"
namespace topcom {
  typedef Array<Vector> matrix_data;
};
#else
#include <vector>
namespace topcom {
  typedef std::vector<Vector> matrix_data;
}
#endif

namespace topcom {

  class Matrix : public matrix_data {
  private:
    static const char start_char;
    static const char end_char;
    static const char delim_char;
  public:
    static const char col_delim_char;
  public:
    static const parameter_type col_capacity;
  public:

    // constructors:
    inline Matrix();
    inline Matrix(const Matrix&);
    inline Matrix(Matrix&&);
    inline Matrix(const Matrix&,
		  const parameter_type min_col,
		  const parameter_type max_col);
    inline Matrix(const Matrix&,
		  const IntegerSet&);
    inline Matrix(const Matrix&,
		  const IntegerSet64&);
    inline Matrix(const Vector&);
    inline Matrix(const parameter_type,
		  const parameter_type,
		  const Field& init_entry = FieldConstants::ZERO);
    inline Matrix(const Permutation& perm);

    // destructor:
    inline ~Matrix();

    // assignment:
    inline Matrix& operator=(const Matrix&);
    inline Matrix& operator=(Matrix&&);

    // accessors:
    inline const parameter_type rowdim() const;
    inline const parameter_type coldim() const;
    const        Vector row(const parameter_type) const;

    // resizing:
    void        row_resize(const parameter_type, const Field& init_entry = FieldConstants::ZERO);
    inline void col_resize(const parameter_type, const Field& init_entry = FieldConstants::ZERO);

    // indexing:
    inline       Vector& col(const parameter_type);
    inline const Vector& col(const parameter_type) const;
    inline       Field& operator()(const parameter_type, const parameter_type);
    inline const Field& operator()(const parameter_type, const parameter_type) const;

    // in place operations:
    Matrix& canonicalize();
    Matrix& add(const Matrix&);
    Matrix& scale(const Field&);
    Matrix& augment(const Matrix&, const IntegerSet& = IntegerSet());
    Matrix& stack(const Matrix&);
    Matrix& swap_cols(const parameter_type, const parameter_type);
    Matrix& swap_rows(const parameter_type, const parameter_type);
    Matrix& row_echelon_form(const parameter_type start_row = 0, const parameter_type start_col = 0, const Field& scale = 1);

    // out of place operations:
    const        Field left_upper_det() const;
    inline const Field det() const;
    Matrix transpose() const;
    friend const Matrix multiply(const Matrix&, const Matrix&);

    // transformation:
    Vector StackOfAllColumns() const;

    // keys for containers:
    inline const size_type keysize() const;
    inline const size_type key(const size_type) const;

    // iostream:
    std::ostream& pretty_print(std::ostream& ost) const;

    // overload of istream with canonicalize:
    std::istream& read(std::istream&);
    inline friend std::istream& operator>>(std::istream&, Matrix&);
    std::ostream& write(std::ostream&) const;
    inline friend std::ostream& operator<<(std::ostream&, const Matrix&);
  };

  inline Matrix::Matrix() : matrix_data() {
    reserve(col_capacity);
  }

  inline Matrix::Matrix(const Matrix& matrix) : matrix_data(matrix) {}

  inline Matrix::Matrix(Matrix&& matrix) : matrix_data(std::move(matrix)) {}

  inline Matrix::Matrix(const Matrix& matrix, parameter_type min_col, parameter_type max_col) :
#ifndef STL_CONTAINERS
    matrix_data(matrix, min_col, max_col)
#else
    matrix_data(matrix.begin() + std::min<parameter_type>(min_col, matrix.coldim()),
		matrix.begin() + std::min<parameter_type>(max_col, matrix.coldim()))
#endif
  {}

  inline Matrix::Matrix(const Matrix& matrix, const IntegerSet& colsubset) :
    matrix_data() {
    parameter_type colidx = 0;
    const parameter_type m_coldim = matrix.coldim();
    for (IntegerSet::const_iterator iter = colsubset.begin();
	 iter != colsubset.end();
	 ++iter) {
      if (*iter < m_coldim) {
	this->push_back(matrix.at(*iter));
	++colidx;
      }
      else {
	break;
      }
    }
  }

  inline Matrix::Matrix(const Matrix& matrix, const IntegerSet64& colsubset) :
    matrix_data() {
    parameter_type colidx = 0;
    const parameter_type m_coldim = matrix.coldim();
    for (IntegerSet64::const_iterator iter = colsubset.begin();
	 iter != colsubset.end();
	 ++iter) {
      if (*iter < m_coldim) {
	this->push_back(matrix.at(*iter));
	++colidx;
      }
      else {
	break;
      }
    }
  }

  inline Matrix::Matrix(const Vector& vector) : matrix_data(1, vector) {
  
  }
  
  inline Matrix::Matrix(const parameter_type init_rowdim, 
			const parameter_type init_coldim, 
			const Field& init_entry) : 
    matrix_data(init_coldim) {
    for (parameter_type i = 0; i < init_coldim; ++i) {
      this->col(i) = Vector(init_rowdim, init_entry);
    }
  }
  
  inline Matrix::Matrix(const Permutation& perm) : matrix_data(perm.n(), Vector(perm.k(), FieldConstants::ZERO)) {
    for (parameter_type i = 0; i < perm.k(); ++i) {
      (*this)(i, perm[i]) = FieldConstants::ONE;
    }
  }

  inline Matrix::~Matrix() {
#ifdef CONSTRUCTOR_DEBUG
    std::cerr << "destroying Matrix " << *this << std::endl;
#endif
  }

  // assignment:
  inline Matrix& Matrix::operator=(const Matrix& matrix) {
    if (this == &matrix) {
      return *this;
    }
    matrix_data::operator=(matrix);
    return *this;
  }

  inline Matrix& Matrix::operator=(Matrix&& matrix) {
    if (this == &matrix) {
      return *this;
    }
    matrix_data::operator=(std::move(matrix));
    return *this;
  }

  inline const parameter_type Matrix::rowdim() const {
    if (size() == 0) {
      return 0;
    }
    else {
      return this->col(0).size();
    }
  }
  inline const parameter_type Matrix::coldim() const {
    return size();
  }

  inline void Matrix::col_resize(const parameter_type new_memsize, const Field& init_entry) {
    matrix_data::resize(new_memsize, Vector(rowdim(), init_entry));
  }

  inline Vector& Matrix::col(const parameter_type index){
    return this->at(index);
  }

  inline const Vector& Matrix::col(const parameter_type index) const {
    return this->at(index);
  }

  inline Field& Matrix::operator()(const parameter_type row, const parameter_type col) {
    return this->at(col).at(row);
  }

  inline const Field& Matrix::operator()(const parameter_type row, const parameter_type col) const {
    return this->at(col).at(row);
  }

  inline const Field Matrix::det() const {
#ifdef INDEX_CHECK
    assert(coldim() == rowdim());
#endif
    return this->left_upper_det();
  }

  // keys for containers:
  inline const size_type Matrix::keysize() const {
    return (size_type)coldim() * (size_type)rowdim();
  }

  inline const size_type Matrix::key(const size_type n) const {
    return hash<Field>()((*this)(n % (size_type)coldim(), n / (size_type)rowdim()));
  }

  inline std::istream& operator>>(std::istream& ist, Matrix& m) {
    m.read(ist);
    return ist;
  }

  inline std::ostream& operator<<(std::ostream& ost, const Matrix& m) {
    m.write(ost);
    return ost;
  }

  // special matrices:
  inline Matrix identity_matrix(const parameter_type dim) {
    return Matrix(Permutation(dim, dim));
  }


}; // namespace topcom

#endif

// eof Matrix.hh
