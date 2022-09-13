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
#include "StrictStairCaseMatrix.hh"
#include "StrictStairCaseMatrixTrans.hh"
#include "Symmetry.hh"
#include "SimplicialComplex.hh"


namespace topcom {

  const int PointConfiguration::zeroes_in_row(const size_type index) const {
    int result = 0;
    for (size_type i = 0; i < coldim(); ++i) {
      if ((*this)(index,i) == FieldConstants::ZERO) {
	++result;
      }
    }
    return result;
  }

  const int PointConfiguration::zeroes_in_col(const size_type index) const {
    int result = 0;
    for (size_type i = 0; i < rowdim(); ++i) {
      if ((*this)(i,index) == FieldConstants::ZERO) {
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
      result += volume(*iter);
    }
    return result;
  }

  const Field PointConfiguration::volume(const Simplex& simp) const {
    StairCaseMatrix basis_matrix;
    for (Simplex::const_iterator iter = simp.begin();
	 iter != simp.end();
	 ++iter) {
      basis_matrix.augment(this->col(*iter));
    }
    return abs(basis_matrix.det());
  }
  
  PointConfiguration& PointConfiguration::prism() {
    PointConfiguration new_cols(*this);
    for (size_type i = 0; i < no(); ++i) {
      this->col(i).push_back(FieldConstants::ZERO);
      new_cols.col(i).push_back(FieldConstants::ONE);
    }
#ifndef STL_CONTAINERS
    push_back(new_cols);
#else
    insert(end(), new_cols.begin(), new_cols.end());
#endif
    return *this;
  }

  PointConfiguration& PointConfiguration::pyramid() {
    Vector new_col(rank(), FieldConstants::ZERO);
    for (size_type i = 0; i < no(); ++i) {
      this->col(i).push_back(FieldConstants::ZERO);
    }
    new_col.push_back(FieldConstants::ONE);
    push_back(new_col);
    return *this;
  }

  PointConfiguration& PointConfiguration::direct_sum(const PointConfiguration& p) {
    PointConfiguration new_cols = Matrix(rank(), p.no(), FieldConstants::ZERO);
    for (size_type i = 0; i < no(); ++i) {
#ifndef STL_CONTAINERS
      this->(i).push_back(Vector(p.rank(), FieldConstants::ZERO));
#else
      Vector extension(p.rank(), FieldConstants::ZERO);
      this->col(i).insert(this->col(i).end(), extension.begin(), extension.end()); 
#endif
    }
    for (size_type i = 0; i < p.no(); ++i) {
#ifndef STL_CONTAINERS
      new_cols.col(i).push_back(p.col(i));
#else
      new_cols.col(i).insert(new_cols.col(i).end(), p.col(i).begin(), p.col(i).end());
#endif
    }
#ifndef STL_CONTAINERS
    push_back(new_cols);
#else
    insert(end(), new_cols.begin(), new_cols.end());
#endif
    return *this;
  }

  PointConfiguration& PointConfiguration::homogenize() {
    stack(Matrix(1, no(), FieldConstants::ONE));
    return *this;
  }

  PointConfiguration& PointConfiguration::transform_to_full_rank() {
    pretty_print(std::cerr);
    Matrix::row_echelon_form();
    return *this;
  }

  PointConfiguration PointConfiguration::gale() const {
    if (coldim() == 0) {
      return *this;
    }
    StrictStairCaseMatrixTrans eliminator;
    eliminator.augment(*this);
    const Matrix kernel(eliminator.transformation(), rowdim(), coldim());
    if (CommandlineOptions::debug()) {
      std::cerr << "points:" << std::endl;
      pretty_print(std::cerr);
      std::cerr << "kernel:" << std::endl;    
      kernel.pretty_print(std::cerr);
      std::cerr << "points * kernel = " << multiply(*this, kernel) << std::endl;
    }
    return PointConfiguration(kernel.transpose());
  }

  PointConfiguration PointConfiguration::product(const PointConfiguration& p) const {
    PointConfiguration result = Matrix(0, no() * p.no());
    const ProductIndex product_index(no(), p.no());
    if ((no() == 0) || (p.no() == 0)) {
      return result;
    }
    for (size_type i = 0; i < no(); ++i) {
      for (size_type j = 0; j < p.no(); ++j) {
	result.col(product_index(i,j)) = this->col(i);
#ifndef STL_CONTAINERS
	result[product_index(i,j)].push_back(p[j]);
#else
	result.col(product_index(i,j)).insert(result.col(product_index(i,j)).end(), p.col(j).begin(), p.col(j).end());
#endif
      }
    }
    return result;
  } 

  PointConfiguration& PointConfiguration::sort_rows() {
    for (size_type i = 0; i < rowdim(); ++i) {
      for (size_type j = i + 1; j < rowdim(); ++j) {
	if (zeroes_in_row(j) < zeroes_in_row(i)) {
	  swap_rows(i, j);
	}
      }
    }
    return *this;
  }

  PointConfiguration& PointConfiguration::lex_abs_sort_cols(Symmetry& sym) {
    std::cerr << "sorting the points " << std::endl;
    pretty_print(std::cerr);
    std::cerr << "with transformation permutation " << sym << std::endl;
    Symmetry new_sym(no());
    for (size_type i = 0; i < no(); ++i) {
      for (size_type j = i + 1; j < no(); ++j) {
	if (lex_abs_compare(this->col(j), this->col(i))) {
	  swap_cols(i, j);
	  new_sym.transpose(i, j);
	}
      }
    }
    std::cerr << "resulting in " << std::endl;
    pretty_print(std::cerr);
    std::cerr << "with transformation permutation " << new_sym << std::endl;
    sym = sym * new_sym;
    std::cerr << "with total permutation of " << sym << std::endl;
    return *this;
  }

  // preprocessing the points can help the (co-)circuits enumeration
  // to start with easy eliminations in the beginning and hoping
  // that later combinations with more numbers are covered by symmetries:
  PointConfiguration& PointConfiguration::preprocess(Symmetry& sym) {
    // Symmetry new_sym(no());
    std::cerr << "preprocessing the points ..." << std::endl;
    sort_rows();
    row_echelon_form();
    // lex_abs_sort_cols(new_sym);
    // reverse(new_sym);
    // sym = sym * new_sym;
    // std::cerr << "... done - resulting transformation permutation " << sym << std::endl;
    std::cerr << "resulting in " << std::endl;
    pretty_print(std::cerr);
    return *this;
  }

  PointConfiguration& PointConfiguration::reverse(Symmetry& sym) {
    Symmetry new_sym(Symmetry(no()).reverse());
    std::cerr << "reversing the points " << std::endl;
    pretty_print(std::cerr);
    std::cerr << "with transformation permutation " << sym << std::endl;
    PointConfiguration tmp(*this);
    for (size_type i = 0; i < tmp.no(); ++i) {
      this->col(coldim() - i - 1) = tmp.col(i);
    }
    std::cerr << "resulting in " << std::endl;
    pretty_print(std::cerr);
    std::cerr << "with transformation permutation " << new_sym << std::endl;
    sym = sym * new_sym;
    std::cerr << "with total permutation of " << sym << std::endl;
    return *this;
  }

  const parameter_type PointConfiguration::_compute_rank() const {
    if (empty()) {
      return 0;
    }
    parameter_type rank(0);
    parameter_type i(0);
    StrictStairCaseMatrix transformed_points(this->col(i));
    if (!this->col(i).is_zero()) {
      ++rank;
    }
    ++i;
    while ((i < coldim()) && (rank < rowdim())) {
      transformed_points.augment(this->col(i));
      rank = transformed_points.rank();
      ++i;
    }
    return rank;
  }

}; // namespace topcom

// eof PointConfiguration.cc
