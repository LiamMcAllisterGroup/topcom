////////////////////////////////////////////////////////////////////////////////
// 
// StairCaseMatrix.hh 
//
//    produced: 21/08/97 jr
// last change: 30/10/97 jr
//
////////////////////////////////////////////////////////////////////////////////
#ifndef STRICTSTAIRCASEMATRIX_HH
#define STRICTSTAIRCASEMATRIX_HH

#include <assert.h>
#include <limits>

#include "CommandlineOptions.hh"

#include "Vector.hh"
#include "StairCaseMatrix.hh"

namespace topcom {

  class StrictStairCaseMatrix : public StairCaseMatrix {
  public:
    // constructors:
    inline StrictStairCaseMatrix();
    inline StrictStairCaseMatrix(const StrictStairCaseMatrix&);
    inline StrictStairCaseMatrix(StrictStairCaseMatrix&&);
    inline StrictStairCaseMatrix(const StrictStairCaseMatrix&,
				 const parameter_type min_col,
				 const parameter_type max_col);
    inline StrictStairCaseMatrix(const Vector&);
    inline StrictStairCaseMatrix(const parameter_type, 
				 const parameter_type);
    // assignment:
    inline StrictStairCaseMatrix& operator=(const StrictStairCaseMatrix&);
    inline StrictStairCaseMatrix& operator=(StrictStairCaseMatrix&&);

    // in place operations:
    StrictStairCaseMatrix& augment(const Matrix&);

    // out of place operations:
    inline bool           has_full_rank() const;
    inline parameter_type rank         () const;
    const Field           valuation    (const Vector&) const; // determinant form induced by the first rowdim - 1 columns

    // iostream:
    std::ostream& pretty_print(std::ostream& ost) const;
    friend std::ostream& operator<<(std::ostream& ost, const StrictStairCaseMatrix& matrix);
  private:
    // internal elimination step:
    void _eliminate(const parameter_type, const parameter_type, const parameter_type);

    // forbid in place operations if result is of type StairCaseMatrix:
    inline StrictStairCaseMatrix& add(const StrictStairCaseMatrix&) = delete;
    inline StrictStairCaseMatrix& scale(const Field&) = delete;
    inline StrictStairCaseMatrix& stack(const StrictStairCaseMatrix&) = delete;
    inline StrictStairCaseMatrix& swap_cols(const parameter_type, const parameter_type) = delete;
    inline StrictStairCaseMatrix& swap_rows(const parameter_type, const parameter_type) = delete;
  };

  inline StrictStairCaseMatrix::StrictStairCaseMatrix() : 
    StairCaseMatrix() {
  }
  inline StrictStairCaseMatrix::StrictStairCaseMatrix(const StrictStairCaseMatrix& matrix) : 
    StairCaseMatrix(matrix) {
  }
  inline StrictStairCaseMatrix::StrictStairCaseMatrix(StrictStairCaseMatrix&& matrix) : 
    StairCaseMatrix(std::move(matrix)) {
  }
  inline StrictStairCaseMatrix::StrictStairCaseMatrix(const StrictStairCaseMatrix& matrix,
						      const parameter_type min_col,
						      const parameter_type max_col) : 
    StairCaseMatrix(matrix, min_col, max_col) {
  }
  inline StrictStairCaseMatrix::StrictStairCaseMatrix(const Vector& vector) :
    StairCaseMatrix(vector) {
  }
  inline StrictStairCaseMatrix::StrictStairCaseMatrix(const parameter_type init_rows, 
						      const parameter_type init_cols) :
    StairCaseMatrix(init_rows, init_cols) {
  }

  // assignment:
  inline StrictStairCaseMatrix& StrictStairCaseMatrix::operator=(const StrictStairCaseMatrix& matrix) {
    if (matrix == *this) {
      return *this;
    }
    StairCaseMatrix::operator=(matrix);
    return *this;
  }
  inline StrictStairCaseMatrix& StrictStairCaseMatrix::operator=(StrictStairCaseMatrix&& matrix) {
    if (matrix == *this) {
      return *this;
    }
    StairCaseMatrix::operator=(std::move(matrix));
    return *this;
  }

  inline bool StrictStairCaseMatrix::has_full_rank() const {
    return StairCaseMatrix::has_no_zerocol();
  }

  inline parameter_type StrictStairCaseMatrix::rank() const {
    parameter_type result(0);
    for (parameter_type colidx = 0; colidx < coldim(); ++colidx) {
      if (!(*this)[colidx].is_zero()) {
	++result;
      }
      else {
	break;
      }
    }
    return result;
  }

}; // namespace topcom

#endif

// eof StairCaseMatrix.hh
