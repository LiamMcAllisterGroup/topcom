////////////////////////////////////////////////////////////////////////////////
// 
// PointConfiguration.cc
//
//    produced: 13/03/98 jr
// last change: 13/03/98 jr
// 
////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <ctype.h>
#include <string.h>

#include "PointConfiguration.hh"
#include "Chirotope.hh"
#include "PlacingTriang.hh"
#include "StairCaseMatrix.hh"

const int PointConfiguration::zeroes_in_row(const size_type index) const {
  int result = 0;
  for (size_type i = 0; i < coldim(); ++i) {
    if ((*this)(index,i) == ZERO) {
      ++result;
    }
  }
  return result;
}

const int PointConfiguration::zeroes_in_col(const size_type index) const {
  int result = 0;
  for (size_type i = 0; i < rowdim(); ++i) {
    if ((*this)(i,index) == ZERO) {
      ++result;
    }
  }
  return result;
}

const Field PointConfiguration::volume() const {
  Field result(0);
  const PlacingTriang pt(Chirotope(*this, false));
  for (SimplicialComplex::const_iterator iter = pt.begin();
       iter != pt.end();
       ++iter) {
    const basis_type& basis(*iter);
    StairCaseMatrix basis_matrix;
    for (basis_type::const_iterator iter = basis.begin();
	 iter != basis.end();
	 ++iter) {
      basis_matrix.augment((*this)[*iter]);
    }
    result += abs(det(basis_matrix));
  }
  return result;
}

PointConfiguration& PointConfiguration::prism() {
  PointConfiguration new_cols(*this);
  for (size_type i = 0; i < no(); ++i) {
    (*this)[i].append(ZERO);
    new_cols[i].append(ONE);
  }
  append(new_cols);
  return *this;
}

PointConfiguration& PointConfiguration::pyramid() {
  Vector new_col(rank(), ZERO);
  for (size_type i = 0; i < no(); ++i) {
    (*this)[i].append(ZERO);
  }
  new_col.append(ONE);
  append(new_col);
  return *this;
}

PointConfiguration& PointConfiguration::direct_sum(const PointConfiguration& p) {
  PointConfiguration new_cols = Matrix(rank(), p.no(), ZERO);
  for (size_type i = 0; i < no(); ++i) {
    (*this)[i].append(Vector(p.rank(), ZERO));
  }
  for (size_type i = 0; i < p.no(); ++i) {
    new_cols[i].append(p[i]);
  }
  append(new_cols);
  return *this;
}

PointConfiguration& PointConfiguration::homogenize() {
  stack(Matrix(1, no(), ONE));
  return *this;
}

PointConfiguration& PointConfiguration::transform_to_full_rank() {
  Matrix::row_normal_form();
  return *this;
}

PointConfiguration PointConfiguration::product(const PointConfiguration& p) const {
  PointConfiguration result = Matrix(0, no() * p.no());
  const ProductIndex product_index(no(), p.no());
  if ((no() == 0) || (p.no() == 0)) {
    return result;
  }
  for (size_type i = 0; i < no(); ++i) {
    for (size_type j = 0; j < p.no(); ++j) {
      result[product_index(i,j)] = (*this)[i];
      result[product_index(i,j)].append(p[j]);
    }
  }
  return result;
} 

PointConfiguration& PointConfiguration::sort_rows() {
  for (size_type i = 0; i < rowdim(); ++i) {
    for (size_type j = i + 1; j < rowdim(); ++j) {
      if (zeroes_in_row(j) > zeroes_in_row(i)) {
	swap_rows(i, j);
      }
    }
  }
  return *this;
}

PointConfiguration& PointConfiguration::lex_abs_sort_cols() {
  for (size_type i = 0; i < coldim(); ++i) {
    for (size_type j = i + 1; j < coldim(); ++j) {
      if (lex_abs_compare((*this)[i],(*this)[j])) {
	swap_cols(i, j);
      }
    }
  }
  return *this;
}

PointConfiguration& PointConfiguration::preprocess() {
  sort_rows();
  lex_abs_sort_cols();
  return *this;
}

// eof PointConfiguration.cc
