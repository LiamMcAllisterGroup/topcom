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

// constructors:

SPXinterface::SPXinterface(const Matrix& m) : 
  _soplex_obj(),
  _lp_obj(),
  _spxpricer_ptr(0),
  _spxtester_ptr(0),
  _spxsolver_ptr(0),
  _spxscaler_ptr(0),
  _spxsimplifier_ptr(0) {
  if (CommandlineOptions::debug()) {
    std::cerr << "building soplex LP ..." << std::endl;
  }

  soplex::Param::setVerbose(0);

  _spxpricer_ptr     = new soplex::SPxDefaultPR();
  _spxtester_ptr     = new soplex::SPxDefaultRT();
  _spxsolver_ptr     = new soplex::SLUFactor();
  _spxscaler_ptr     = 0;
  _spxsimplifier_ptr = 0;

  _soplex_obj.setPricer(_spxpricer_ptr);
  _soplex_obj.setTester(_spxtester_ptr);
  _soplex_obj.setSolver(_spxsolver_ptr);
  _soplex_obj.setScaler(_spxscaler_ptr);
  _soplex_obj.setSimplifier(_spxsimplifier_ptr);

  assert(_soplex_obj.isConsistent());

  // note that the coefficient matrix m is transposed for efficiency reasons;

  for (size_type i = 0; i < m.coldim(); ++i) {
    soplex::DSVector row_vec;

    for (size_type j = 0; j < m.rowdim(); ++j) {

      // first, the row of the coefficient matrix of the LP has to be added;
      // again, we have to use the coefficient matrix m in a transposed way;
      // since soplex uses sparse structures, we just enter non zeroes:
      if (m(j, i) != ZERO) {
	row_vec.add(j, m(j, i));
      }
    }

    
    // we could do the following:
    // next, we must construct a complete LP row from
    // the coefficient vector, the right hand side and the sense;
    // the right hand side is zero but we need strict feasibility,
    // i.e., we seek an interior point of a homogeneous cone Ax > 0;
    // because of homogenity, we can safely set the right hand side to one, 
    // since every solution x with Ax > 0 can be scaled to a solution y with Ay >= 1;
    // moreover, the changing directions Ax <= -1 to Ax >= 1 in all inequalities at the same time
    // leads to an equivalent problem because if y is feasible to Ax <= -1 then
    // -y is feasible to Ax >= 1:
    soplex::LPRow new_row(soplex::Real(1), row_vec, soplex::infinity);
    _lp_obj.addRow(new_row);
  }
  for (size_type j = 0; j < m.rowdim(); ++j) {
    
    // we are just after strict feasibility, so the objective
    // is disabled by setting it to zero everywhere:
    _lp_obj.changeObj(j, soplex::Real(0));

    // it does not make sense to allow for arbitrarily large solutions because
    // of numerical artifacts:
    _lp_obj.changeUpper(j, soplex::Real(1e10));
  }

  // print LP:
  if (CommandlineOptions::debug()) {
    std::cerr << _lp_obj << std::endl;
  }
  _soplex_obj.loadLP(_lp_obj);

  if (CommandlineOptions::debug()) {
    std::cerr << "... done." << std::endl;
  }  
  
  if (CommandlineOptions::debug()) {
    std::cerr << "... done." << std::endl;
  }  
}

// functions:
bool SPXinterface::has_interior_point() {

  static long idx(0);
  
  // Check feasibility with soplex library:
  _soplex_obj.solve();

  bool infeasible(_soplex_obj.status() == _soplex_obj.INFEASIBLE);
  
  if (infeasible) {
    return false;
  }
  else {
    if (CommandlineOptions::output_heights()) {
      std::cout << "beta[" << ++idx << "] := ";
      soplex::Real* solbuf_ptr = new soplex::Real[_soplex_obj.dim()];
      soplex::Vector heights(_soplex_obj.dim(), solbuf_ptr);
      _soplex_obj.getPrimal(heights);
      std::cout << heights << std::endl;
      if (CommandlineOptions::debug()) {
	_soplex_obj.dumpFile("soplex.lp");
      }
      delete[] solbuf_ptr;
    }
  
    return true;
  }

}

#endif // HAVE_LIBSOPLEX

// eof SPXinterface.cc
