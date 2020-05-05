////////////////////////////////////////////////////////////////////////////////
// 
// RegularityCheck.hh 
//
//    produced: 2001/10/20 jr
// last change: 2001/10/20 jr
//
////////////////////////////////////////////////////////////////////////////////
#ifndef REGULARITYCHECK_HH
#define REGULARITYCHECK_HH

#include "Matrix.hh"
#include "PointConfiguration.hh"
#include "SimplicialComplex.hh"
#include "Flip.hh"

#include "CommandlineOptions.hh"

#include "Chirotope.hh"

#include "LPinterface.hh"
#include "SPXinterface.hh"

class RegularityCheck {
private:
  Matrix                    _coeffs;
  const PointConfiguration* _pointsptr;
  const Chirotope*          _chiroptr;
  const SimplicialComplex*  _triangptr;
private:
  RegularityCheck();
public:
  // constructors:
  RegularityCheck(const PointConfiguration& points, 
		  const Chirotope&, 
		  const SimplicialComplex&);
  inline RegularityCheck(const RegularityCheck&);
  inline RegularityCheck(const RegularityCheck&, const Flip&);
  // destructor:
  inline ~RegularityCheck();
  // assignments:
  inline RegularityCheck& operator=(const RegularityCheck&);
  // accessors:
  inline const Matrix     coeffs()   const { return _coeffs; }
  inline const Chirotope* chiroptr() const { return _chiroptr; }
  // functions:
  inline const bool is_regular() const;
};

// constructors:

inline RegularityCheck::RegularityCheck(const RegularityCheck&) {
  std::cerr << "RegularityCheck::RegularityCheck(const RegularityCheck&): not yet implemented!" << std::endl;
}

inline RegularityCheck::RegularityCheck(const RegularityCheck&, const Flip&){
  std::cerr << "RegularityCheck::RegularityCheck(const RegularityCheck&, const Flip&): not yet implemented!" << std::endl;
}

// destructor:
inline RegularityCheck::~RegularityCheck() {}

// assignment:
inline RegularityCheck& RegularityCheck::operator=(const RegularityCheck& rc) {
  if (this == &rc) {
    return *this;
  }
  _coeffs   = rc._coeffs;
  _chiroptr = rc._chiroptr;
  return *this;
}

// functions:
inline const bool RegularityCheck::is_regular() const {
  if (_coeffs.coldim() == 0) {
    if (CommandlineOptions::output_heights()) {
      std::cout << "(";

      // take a height vector that is ZERO on all used points and ONE otherwise:
      const IntegerSet support(_triangptr->support());
      const parameter_type no(_chiroptr->no());
      for (size_type i = 0; i < no - 1; ++i) {
	if (support.contains(i)) {
	  std::cout << ZERO << ", ";
	}
	else {
	  std::cout << ONE << ", ";
	}
      }
      if (support.contains(no)) {
	std::cout << ZERO;
      }
      else {
	std::cout << ONE;
      }
      std::cout << ")" << std::endl;
    }
    return true;
  }

#ifdef HAVE_LIBSOPLEX
  if (CommandlineOptions::use_soplex()) {
    SPXinterface LP(_coeffs);
    const bool regular(LP.has_interior_point());
    if (CommandlineOptions::debug()) {
      LPinterface LP(_coeffs, _triangptr->support());
      const bool double_check_regular(LP.has_interior_point());
      if (regular != double_check_regular) {
	std::cerr << "RegularityCheck::is_regular() const: wrong result, exiting;" << std::endl
		  << "please inspect soplex.lp." << std::endl;
	exit(1);
      }
    }
    return regular;
  }
  else {
#endif
    LPinterface LP(_coeffs, _triangptr->support());
    return LP.has_interior_point();
#ifdef HAVE_LIBSOPLEX
  }
#endif
}

#endif

// eof RegularityCheck.hh
