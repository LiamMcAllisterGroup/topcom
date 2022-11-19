////////////////////////////////////////////////////////////////////////////////
// 
// SPXinterface.cc
//
//    produced: 2003/01/15 jr
// last change: 2003/01/15 jr
//
////////////////////////////////////////////////////////////////////////////////

#ifdef HAVE_LIBSOPLEX

#include "SPXinterface.hh"

namespace topcom {

  // constructors:

  SPXinterface::SPXinterface(const Matrix& m, const LabelSet& support) : 
    _soplex_obj(),
    _support(support) {
    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "building soplex LP ..." << std::endl;
    }

    _soplex_obj.spxout.setVerbosity( soplex::SPxOut::ERROR );

    _soplex_obj.setRealParam(_soplex_obj.FEASTOL, 0);
    _soplex_obj.setRealParam(_soplex_obj.OPTTOL, 0);
    _soplex_obj.setIntParam(_soplex_obj.SOLVEMODE, 2);
    _soplex_obj.setIntParam(_soplex_obj.SYNCMODE, 1);
    _soplex_obj.setIntParam(_soplex_obj.READMODE, 1);
    _soplex_obj.setIntParam(_soplex_obj.CHECKMODE, 2);

    for (size_type i = 0; i < m.coldim(); ++i) {
      soplex::DSVectorRational row_vec;
      bool nonempty = false;

      for (size_type j = 0; j < m.rowdim(); ++j) {

	// first, the row of the coefficient matrix of the LP has to be added;
	// again, we have to use the coefficient matrix m in a transposed way;
	// since soplex uses sparse structures, we just enter non zeroes:
	if (m(j, i) != FieldConstants::ZERO) {
	  row_vec.add(j, __field_to_soplexrational(m(j, i)));
	  nonempty = true;
	}
      }

      // next, we must construct a complete LP row from
      // the coefficient vector, the right hand side and the sense;
      // the right hand side is zero but we need strict feasibility,
      // i.e., we seek an interior point of a homogeneous cone Ax > 0;
      // because of homogenity, we can safely set the right hand side to one, 
      // since every solution x with Ax > 0 can be scaled to a solution y with Ay >= 1;
      // moreover, the changing directions Ax <= -1 to Ax >= 1 in all inequalities at the same time
      // leads to an equivalent problem because if y is feasible for Ax <= -1 then
      // -y is feasible for Ax >= 1:
      if (nonempty) {
	_soplex_obj.addRowRational(soplex::LPRowRational(1, row_vec, soplex::infinity));
      }
    }

    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "... done." << std::endl;
    }  
  }

  // functions:
  bool SPXinterface::has_interior_point(Vector* heightsptr) {

    if (CommandlineOptions::debug()) {
      _soplex_obj.writeFileRational("SPX_LP_debugfile.lp");
    }

    static size_type idx = 0;  
    soplex::SPxSolver::Status stat;
  
    // check feasibility with soplex library:
    stat = _soplex_obj.solve();
    bool infeasible(stat == soplex::SPxSolver::INFEASIBLE);
    if (infeasible) {
      return false;
    }
    else {
      if (CommandlineOptions::output_heights()) {
	soplex::VectorRational heights(_soplex_obj.numCols());
	_soplex_obj.getPrimalRational(heights);
	soplex::Rational maxheight(1);
	for (parameter_type j = 0; j < heights.dim(); j++) {
	  const soplex::Rational x_j = heights[j];
	  if (maxheight - 1 < x_j) {
	    maxheight = x_j + 1;
	  }
	}
	for (parameter_type j = 0; j < heights.dim(); j++) {
	  const soplex::Rational x_j = heights[j];
	  if (_support.contains(j)) {
	    heightsptr->at(j) = __soplexrational_to_field(x_j);
	  }
	  else {
	    if (CommandlineOptions::debug()) {
	      std::cerr << std::endl << "-- point " << j << " unused, assigning height " << maxheight << " --" << std::endl;
	    }
	    heightsptr->at(j) = __soplexrational_to_field(maxheight);
	  }
	}
      }
      return true;
    }

  }

}; // namespace topcom

#endif // HAVE_LIBSOPLEX

// eof SPXinterface.cc
