////////////////////////////////////////////////////////////////////////////////
// 
// RegularityCheck.cc
//
//    produced: 2001/10/28 jr
// last change: 2001/10/28 jr
//
////////////////////////////////////////////////////////////////////////////////
#include "RegularityCheck.hh"

// constructors:

RegularityCheck::RegularityCheck(const PointConfiguration& points,
				 const Chirotope& chiro, 
				 const SimplicialComplex& t) :
  _pointsptr(&points),
  _chiroptr (&chiro),
  _triangptr(&t) {

  // we build the coefficient matrix from the chirotope and the triangulation t:

  // for all simplices in the triangulation t ...:
  for (SimplicialComplex::const_iterator iter1 = t.begin();
       iter1 != t.end();
       ++iter1) {
    const Simplex& simp1(*iter1);
    SimplicialComplex::const_iterator iter2 = iter1;
    while (++iter2 != t.end()) {

      // ... look for an adjacent simplex:
      const Simplex& simp2(*iter2);
      Simplex ridge(simp1);
      ridge *= simp2;
      if (ridge.card() != _chiroptr->rank() - 1) {
	continue;
      }

      // simp2 is adjacent, build new constraint:
      Vector new_col(_chiroptr->no());

      // build the matrix of all points corresponding to simp1:
      StairCaseMatrix basis_matrix;
      for (basis_type::const_iterator basis_iter = simp1.begin();
	   basis_iter !=  simp1.end();
	   ++basis_iter) {
	basis_matrix.augment((*_pointsptr)[*basis_iter]);
      }
      const size_type new_index = *((simp2 - ridge).begin());
      Rational det_basis = det(basis_matrix);
      int calibrate(sign(det_basis));
      new_col[new_index] = calibrate * det_basis;

      // the orientation of simp1 is the calibration according to a point with
      // infinite height:
      calibrate = -calibrate;

      // build the matrices with the point not in simp1 and the facets of simp1:
      basis_type basis(simp1);
      for (Simplex::const_iterator simp_iter = simp1.begin();
	   simp_iter != simp1.end();
	   ++simp_iter) {
	const size_type idx(*simp_iter);
	basis -= idx;
	StairCaseMatrix basis_matrix;
	basis_matrix.append((*_pointsptr)[new_index]);
	for (basis_type::const_iterator basis_iter = basis.begin();
	     basis_iter !=  basis.end();
	     ++basis_iter) {
	  basis_matrix.augment((*_pointsptr)[*basis_iter]);
	}
	new_col[idx] = calibrate * det(basis_matrix);
	calibrate = -calibrate;
	basis += idx;
      }

      // finally, append the new column; it corresponds to 
      // local regularity at ridge (we take a column for efficiency reasons,
      // in cdd it will be a row, of course:
      _coeffs.append(new_col);
    }
  }
  if (CommandlineOptions::debug()) {
    std::cerr << "The coefficient matrix for regularity check:" << std::endl;
    if (_coeffs.coldim() == 0) {
      std::cerr << "no constraints." << std::endl;
    }
    else {
      transpose(_coeffs).pretty_print(std::cout);
    }
  }
}


// eof RegularityCheck.cc
