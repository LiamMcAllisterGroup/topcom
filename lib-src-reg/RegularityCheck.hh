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

#include <thread>
#include <mutex>

#include "Vector.hh"
#include "Matrix.hh"
#include "PointConfiguration.hh"
#include "LabelSet.hh"
#include "SimplicialComplex.hh"
#include "Flip.hh"

#include "CommandlineOptions.hh"

#include "Chirotope.hh"
#include "Incidences.hh"

#include "LPinterface.hh"
#include "SPXinterface.hh"

#ifdef USE_QSO
#include "QSOinterface.hh"
#endif

namespace topcom {

#ifdef REGULARITY_CACHE
  typedef std::pair<Simplex, Simplex> constraint_conf_type;

  class ConstraintCacheEntry : public constraint_conf_type {
  public:
    inline ConstraintCacheEntry() = delete;
    inline ConstraintCacheEntry(const Simplex& unionsimp, const Simplex& intersection) :
      constraint_conf_type(unionsimp, intersection) {}
  public:
    size_type keysize() const { return 2; }
    size_type key(const size_type n) const {
      if (n == 0) {
	return first.key(0);
      }
      else {
	return second.key(0);
      }
    }
  };

  typedef std::unordered_map<ConstraintCacheEntry, Vector, Hash<ConstraintCacheEntry> > constraint_cache_data;
  
  class ConstraintCache : public constraint_cache_data {
  };
#endif

  class RegularityCheck {
  public:
    static size_type cnt_calls;
  private:
    Vector                    _heights;
    Matrix                    _coeffs;
    const PointConfiguration* _pointsptr;
    const Chirotope*          _chiroptr;
    const SimplicialComplex*  _triangptr;
    const Incidences*         _incidencesptr;

#ifdef REGULARITY_CACHE
  private:
    // thread-local caching (map a union of two simplices and their common face
    // to the coefficients of the corresponding folding form:
    static ConstraintCache    _cache;
#endif

  private:
    // multithreading:
    static std::mutex         _reg_mutex;
  private:
    RegularityCheck() = delete;
  public:
    // constructors:
    RegularityCheck(const PointConfiguration&, 
		    const Chirotope&,
		    const Incidences&, 
		    const SimplicialComplex&);
    inline RegularityCheck(const RegularityCheck&);
    inline RegularityCheck(const RegularityCheck&, const Flip&);
    // destructor:
    inline ~RegularityCheck();
    // assignments:
    inline RegularityCheck& operator=(const RegularityCheck&);
    // accessors:
    inline const Vector     heights()  const { return _heights; }   
    inline const Matrix     coeffs()   const { return _coeffs; }
    inline const Chirotope* chiroptr() const { return _chiroptr; }
    // init/term the solver:
    inline static void init();
    inline static void term();
    // functions:
    inline const bool is_regular();  // changes "_heights"
  };

  // constructors:

  inline RegularityCheck::RegularityCheck(const RegularityCheck&) {
    std::cerr << "RegularityCheck::RegularityCheck(const RegularityCheck&): not yet implemented!" << std::endl;
  }

  inline RegularityCheck::RegularityCheck(const RegularityCheck&, const Flip&){
    std::cerr << "RegularityCheck::RegularityCheck(const RegularityCheck&, const Flip&): not yet implemented!" << std::endl;
  }

  // destructor:
  inline RegularityCheck::~RegularityCheck() {
#ifdef CONSTRUCTOR_DEBUG
    std::cerr << "destroying coefficients of RegularityCheck " << this->_coeffs << std::endl;
#endif
  }

  // assignment:
  inline RegularityCheck& RegularityCheck::operator=(const RegularityCheck& rc) {
    if (this == &rc) {
      return *this;
    }
    _heights       = rc._heights;
    _coeffs        = rc._coeffs;
    _pointsptr     = rc._pointsptr;
    _chiroptr      = rc._chiroptr;
    _triangptr     = rc._triangptr;
    _incidencesptr = rc._incidencesptr;
    return *this;
  }

  // functions:
  inline const bool RegularityCheck::is_regular() {
    {
      std::lock_guard<std::mutex> lock(_reg_mutex);
      ++cnt_calls;
    }
    if (_chiroptr->rank() < 3) {
      return true;
    }

    if (_coeffs.coldim() == 0) {
      if (CommandlineOptions::output_heights()) {

	// take a height vector that is ZERO on all used points and ONE otherwise:
	const LabelSet support(_triangptr->support());
	const parameter_type no(_chiroptr->no());
	for (size_type i = 0; i < no; ++i) {
	  if (!support.contains(i)) {
	    _heights.at(i) = FieldConstants::ONE;
	  }
	}
      }
      return true;
    }

#ifdef HAVE_LIBSOPLEX
    if (CommandlineOptions::use_soplex()) {
      SPXinterface LP(_coeffs, _triangptr->support());
      const bool regular(LP.has_interior_point(&_heights));
      if (CommandlineOptions::debug()) {
	LPinterface LP(_coeffs, _triangptr->support());
	const bool double_check_regular(LP.has_interior_point(&_heights));
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
#ifdef USE_QSO
      if (CommandlineOptions::use_qsopt_ex()) {
	// qsopt_ex cannot run in a multi-threaded environment
	// because of its memory management(?);
	// even locking does not suffice,
	// thus parallel enumeration is
	// not supported with qsopt_ex from the start:
	QSOinterface LP(_coeffs, _triangptr->support());
	const bool result = LP.has_interior_point(&_heights);
	return result;
      }
      else
#endif
	{
	// with the patched version of cddlib with TLS
	// we can run the cdd lp solver lock-free:
	LPinterface LP(_coeffs, _triangptr->support());
	const bool result = LP.has_interior_point(&_heights);
	return result;
      }
#ifdef HAVE_LIBSOPLEX
    }
#endif
  }

  // init/term the solver:
  inline void RegularityCheck::init() {
#ifdef USE_QSO
    if (CommandlineOptions::use_qsopt_ex()) {
      QSOinterface::init();
    }
    else
#endif
#ifdef HAVE_LIBSOPLEX
    if (CommandlineOptions::use_soplex()) {
      SPXinterface::init();
    } else
#endif
    {
      LPinterface::init();
    }
  }

  inline void RegularityCheck::term() {
#if USE_QSO
    if (CommandlineOptions::use_qsopt_ex()) {
      QSOinterface::term();
    }
    else
#endif
#ifdef HAVE_LIBSOPLEX
    if (CommandlineOptions::use_soplex()) {
      SPXinterface::term();
    }
    else
#endif
    {
      LPinterface::term();
    }
    if (CommandlineOptions::verbose()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << cnt_calls << " calls of RegularityCheck in total" << std::endl;
    }
  }

}; // namespace topcom

#endif

// eof RegularityCheck.hh
