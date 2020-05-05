////////////////////////////////////////////////////////////////////////////////
// 
// PointConfiguration.hh 
//
//    produced: 21/08/97 jr
// last change: 30/10/97 jr
//
////////////////////////////////////////////////////////////////////////////////
#ifndef POINTCONFIGURATION_HH
#define POINTCONFIGURATION_HH

#include <assert.h>

#include "Vector.hh"
#include "Matrix.hh"
#include "StairCaseMatrix.hh"

#include "CommandlineOptions.hh"

typedef Vector Point;

class ProductIndex {
private:
  const size_type _size1;
  const size_type _size2;
private:
  ProductIndex();
public:
  inline ProductIndex(const ProductIndex& pi) :
    _size1(pi._size1), _size2(pi._size2) {}
  inline ProductIndex(const size_type size1, const size_type size2) :
    _size1(size1), _size2(size2) {}
  inline const size_type operator()(const size_type i1, const size_type i2) const {
    return i1*_size2 + i2;
  }
};

class CyclicIndex {
private:
  const size_type _size1;
  const size_type _size2;
private:
  CyclicIndex();
public:
  inline CyclicIndex(const CyclicIndex& pi) :
    _size1(pi._size1), _size2(pi._size2) {}
  inline CyclicIndex(const size_type size1, const size_type size2) :
    _size1(size1), _size2(size2) {}
  inline const size_type operator()(const size_type i1, const size_type i2) const {
    return i1;
  }
};

class RevCyclicIndex {
private:
  const size_type _size1;
  const size_type _size2;
private:
  RevCyclicIndex();
public:
  inline RevCyclicIndex(const RevCyclicIndex& pi) :
    _size1(pi._size1), _size2(pi._size2) {}
  inline RevCyclicIndex(const size_type size1, const size_type size2) :
    _size1(size1), _size2(size2) {}
  inline const size_type operator()(const size_type i1, const size_type i2) const {
    return i2;
  }
};

class PointConfiguration : public Matrix {
public:
  // constructors:
  inline PointConfiguration();
  inline PointConfiguration(const PointConfiguration&);
  inline PointConfiguration(const Matrix&);
  // accessors:
  inline const parameter_type no()   const { return coldim(); }
  inline const parameter_type rank() const { return rowdim(); }  
  const int zeroes_in_row(const size_type) const;
  const int zeroes_in_col(const size_type) const;
  // functions
  const Field volume() const;
  // constructions in place:
  PointConfiguration& prism      ();
  PointConfiguration& pyramid    ();
  PointConfiguration& direct_sum (const PointConfiguration&);
  PointConfiguration& homogenize ();
  PointConfiguration& transform_to_full_rank();
  // constructions out of place:
  PointConfiguration  product    (const PointConfiguration&) const;
  // operations in place:
  PointConfiguration& sort_rows();
  PointConfiguration& lex_abs_sort_cols();
  PointConfiguration& preprocess();
  // std::istream:
  inline std::istream& read(std::istream&);
  inline friend std::istream& operator>>(std::istream&, PointConfiguration&);
};

inline PointConfiguration::PointConfiguration() : 
  Matrix() {
}
inline PointConfiguration::PointConfiguration(const PointConfiguration& points) : 
  Matrix(points) {
}
inline PointConfiguration::PointConfiguration(const Matrix& matrix) : 
  Matrix(matrix) {
}

inline std::istream& PointConfiguration::read(std::istream& ist) {
  Matrix::read(ist);
  this->canonicalize();
  for (size_type i = 0; i < this->maxindex(); ++i) {
    if ((*this)[i].is_zero()) {
#ifdef READ_DEBUG
      std::cerr << "PointConfiguration::read(std::istream& ist): "
		<< "[0, ..., 0] not allowed."
		<< std::endl;
#endif
      ist.setstate(std::ios::failbit);
      return ist;
    }
  }
  return ist;
}

inline std::istream& operator>>(std::istream& ist, PointConfiguration& pc) {
  return pc.read(ist);
}

#endif

// eof PointConfiguration.hh
