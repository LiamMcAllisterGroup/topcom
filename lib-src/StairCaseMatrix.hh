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
#include <limits>

#include "CommandlineOptions.hh"

#include "Vector.hh"
#include "Matrix.hh"

namespace topcom {

  class StairCaseMatrix : public Matrix {
  protected:
    Field _coefficient;
  public:
    
    // constructors:
    inline StairCaseMatrix();
    inline StairCaseMatrix(const StairCaseMatrix&);
    inline StairCaseMatrix(StairCaseMatrix&&);
    inline StairCaseMatrix(const StairCaseMatrix&,
			   const parameter_type,
			   const parameter_type);
    inline StairCaseMatrix(const Matrix&,
			   const IntegerSet& = IntegerSet());
    inline StairCaseMatrix(const Vector&);
    inline StairCaseMatrix(const parameter_type, 
			   const parameter_type, 
			   const Field& init_entry = FieldConstants::ZERO);

    // destructor:
    inline ~StairCaseMatrix();
    
    // assignment:
    inline StairCaseMatrix& operator=(const StairCaseMatrix&);
    inline StairCaseMatrix& operator=(StairCaseMatrix&&);
    
    // accessors:
    inline const Field& coefficient() const;
    
    // in place operations:
    StairCaseMatrix& augment(const Matrix&, const IntegerSet& = IntegerSet());
    
    // out of place operations:
    inline bool has_no_zerocol() const;
    const Field left_upper_det() const;
    const Field det()            const;
    
    // iostream:
    std::ostream& pretty_print(std::ostream&) const;
    friend std::ostream& operator<<(std::ostream&, const StairCaseMatrix&);
  private:
    
    // internal elimination step:
    void _eliminate(const parameter_type, const parameter_type, const parameter_type);
    
    // forbid in place operations if result is of type StairCaseMatrix:
    inline StairCaseMatrix& add(const StairCaseMatrix&) = delete;
    inline StairCaseMatrix& scale(const Field&) = delete;
    inline StairCaseMatrix& stack(const StairCaseMatrix&) = delete;
  };

  inline StairCaseMatrix::StairCaseMatrix() : 
    Matrix(), _coefficient(FieldConstants::ONE) {
  }

  inline StairCaseMatrix::StairCaseMatrix(const StairCaseMatrix& matrix) :
    Matrix(matrix), _coefficient(matrix._coefficient) {
  }

  inline StairCaseMatrix::StairCaseMatrix(StairCaseMatrix&& matrix) : 
    Matrix(std::move(matrix)), _coefficient(std::move(matrix._coefficient)) {
  }

  inline StairCaseMatrix::StairCaseMatrix(const StairCaseMatrix& matrix,
					  const parameter_type min_col,
					  const parameter_type max_col) : 
    Matrix(matrix, min_col, max_col), _coefficient(matrix._coefficient) {
  }

  inline StairCaseMatrix::StairCaseMatrix(const Matrix&     matrix,
					  const IntegerSet& ignored_cols) :
    Matrix(), _coefficient(FieldConstants::ONE) {
    this->augment(matrix, ignored_cols);
  }
    
  
  inline StairCaseMatrix::StairCaseMatrix(const Vector& vector) : 
    Matrix(vector), _coefficient(FieldConstants::ONE) {
  }

  inline StairCaseMatrix::StairCaseMatrix(const parameter_type init_rows, 
					  const parameter_type init_cols, 
					  const Field& init_entry) :
    Matrix() , _coefficient(FieldConstants::ONE) {
    if (init_entry == FieldConstants::ZERO) {
      Matrix::operator=(Matrix(init_rows, init_cols, init_entry));
    }
    else {
      this->augment(Matrix(init_rows, init_cols, init_entry));
    }
  }
  inline StairCaseMatrix::~StairCaseMatrix() {
  }
  inline StairCaseMatrix& StairCaseMatrix::operator=(const StairCaseMatrix& matrix) {
    if (this == &matrix) {
      return *this;
    }
    Matrix::operator=(matrix);
    _coefficient = matrix._coefficient;
    return *this;
  }
  inline StairCaseMatrix& StairCaseMatrix::operator=(StairCaseMatrix&& matrix) {
    if (this == &matrix) {
      return *this;
    }
    Matrix::operator=(std::move(matrix));
    _coefficient = std::move(matrix._coefficient);
    return *this;
  }
  inline const Field& StairCaseMatrix::coefficient() const {
    return _coefficient;
  }
  inline bool StairCaseMatrix::has_no_zerocol() const {
    if (coldim() == 0) {
      return true;
    }
    else {
      return (!this->col(coldim() - 1).is_zero());
    }
  }

}; // namespace topcom

#endif

// eof StairCaseMatrix.hh
