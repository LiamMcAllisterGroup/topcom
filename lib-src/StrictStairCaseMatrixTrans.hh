////////////////////////////////////////////////////////////////////////////////
// 
// StairCaseMatrix.hh 
//
//    produced: 21/08/97 jr
// last change: 30/10/97 jr
//
////////////////////////////////////////////////////////////////////////////////
#ifndef STRICTSTAIRCASEMATRIXTRANS_HH
#define STRICTSTAIRCASEMATRIXTRANS_HH

#include <assert.h>
#include <limits>

#include "CommandlineOptions.hh"

#include "Vector.hh"
#include "StairCaseMatrix.hh"

namespace topcom {

  class StrictStairCaseMatrixTrans : public StairCaseMatrix {
  private:
    // for each resulting column, we store the coefficients
    // of its representation as a linear combination
    // of original columns;
    // that is: the first coldim rows of the transformation matrix
    // times the original matrix
    // equals the resulting strict staircase matrix;
    // whenever the last column is transformed to zero,
    // its transformation contains an element of the kernel
    // of the original matrix
    // this happens no later than we have one more columns than rows;
    // thus, the transformation matrix needs at most as many rows
    // as the original matrix has rows plus one:
    Matrix _transformation;
  public:
    // constructors:
    inline StrictStairCaseMatrixTrans();
    inline StrictStairCaseMatrixTrans(const StrictStairCaseMatrixTrans&);
    inline StrictStairCaseMatrixTrans(StrictStairCaseMatrixTrans&&);
    inline StrictStairCaseMatrixTrans(const Vector&);
    inline StrictStairCaseMatrixTrans(const parameter_type, 
				      const parameter_type);

    // destructor:
    inline ~StrictStairCaseMatrixTrans();
    
    // assignment:
    inline StrictStairCaseMatrixTrans& operator=(const StrictStairCaseMatrixTrans&);
    inline StrictStairCaseMatrixTrans& operator=(StrictStairCaseMatrixTrans&&);

    // accessors:
    inline const Matrix& transformation() const;

    // in place operations:
    StrictStairCaseMatrixTrans& augment(const Vector&);
    StrictStairCaseMatrixTrans& augment(const Matrix&);

    // out of place operations:
    inline bool      has_full_rank() const;
    inline parameter_type rank         () const;
    const Field      valuation    (const Vector&) const; // determinant form induced by the first rowdim - 1 columns

    // iostream:
    std::ostream& pretty_print(std::ostream& ost) const;
    friend std::ostream& operator<<(std::ostream& ost, const StrictStairCaseMatrixTrans& matrix);
  private:
    // internal elimination step:
    void _eliminate(const parameter_type, const parameter_type, const parameter_type);

    // forbid in place operations if result is of type StairCaseMatrix:
    inline StrictStairCaseMatrixTrans& add(const StrictStairCaseMatrixTrans&) = delete;
    inline StrictStairCaseMatrixTrans& scale(const Field&) = delete;
    inline StrictStairCaseMatrixTrans& stack(const StrictStairCaseMatrixTrans&) = delete;
  };

  inline StrictStairCaseMatrixTrans::StrictStairCaseMatrixTrans() : 
    StairCaseMatrix(), _transformation() {
  }
  inline StrictStairCaseMatrixTrans::StrictStairCaseMatrixTrans(const StrictStairCaseMatrixTrans& matrix) : 
    StairCaseMatrix(matrix), _transformation(matrix._transformation) {
  }
  inline StrictStairCaseMatrixTrans::StrictStairCaseMatrixTrans(StrictStairCaseMatrixTrans&& matrix) : 
    StairCaseMatrix(std::move(matrix)), _transformation(std::move(matrix._transformation)) {
  }
  inline StrictStairCaseMatrixTrans::StrictStairCaseMatrixTrans(const Vector& vector) :

    // initialize with one column and the identity transformation
    // that this column equals FieldConstants::ONE times the first column plus
    // FieldConstants::ZERO times the remaining (not yet existing) columns:
    StairCaseMatrix(vector), _transformation(vector.dim() + 1, 1, FieldConstants::ZERO) {
    _transformation(0, 0) = FieldConstants::ONE;
  }
  inline StrictStairCaseMatrixTrans::StrictStairCaseMatrixTrans(const parameter_type init_rows, 
								const parameter_type init_cols) :

    // initialize with a matrix of init_cols columns with init_rows rows each
    // and a transformation matrix with the first init_cols columns of the
    // (init_rows + 1) x (init_rows + 1) identity matrix;
    // meaning: each column equals FieldConstants::ONE times itself plus
    // FieldConstants::ZERO times the remaining (maybe not yet existing) columns;
    // there will be never more than init_rows + 1 columns necessary
    // to generate a zero-column:
    StairCaseMatrix(init_rows, init_cols), _transformation(init_rows + 1, init_cols, FieldConstants::ZERO) {
    assert(init_cols <= init_rows);
    for (parameter_type j = 0; j < init_cols; ++j) {
      _transformation(j, j) = FieldConstants::ONE;
    }
  }

  // desctructor:
  inline StrictStairCaseMatrixTrans::~StrictStairCaseMatrixTrans() {
  }

  // assignment:
  inline StrictStairCaseMatrixTrans& StrictStairCaseMatrixTrans::operator=(const StrictStairCaseMatrixTrans& matrix) {
    if (matrix == *this) {
      return *this;
    }
    StairCaseMatrix::operator=(matrix);
    _transformation = matrix._transformation;
    return *this;
  }
  inline StrictStairCaseMatrixTrans& StrictStairCaseMatrixTrans::operator=(StrictStairCaseMatrixTrans&& matrix) {
    if (matrix == *this) {
      return *this;
    }
    StairCaseMatrix::operator=(std::move(matrix));
    _transformation = std::move(matrix._transformation);
    return *this;
  }

  // accessors:
  inline const Matrix& StrictStairCaseMatrixTrans::transformation() const {
    return _transformation;
  }

  inline bool StrictStairCaseMatrixTrans::has_full_rank() const {
    return StairCaseMatrix::has_no_zerocol();
  }

  inline parameter_type StrictStairCaseMatrixTrans::rank() const {
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

// eof StrictStairCaseMatrixTrans.hh
