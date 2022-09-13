//////////////////////////////////////////////////////////////////////////
// ClassifiedExtensionSymmetries.hh
// produced: 22/03/2020 jr
// last change: 22/03/2020 jr
/////////////////////////////////////////////////////////////////////////
#ifndef CLASSIFIEDEXTENSIONSYMMETRIES_HH
#define CLASSIFIEDEXTENSIONSYMMETRIES_HH

#include <assert.h>
#include <iostream>
#include <iterator>

#include <vector>
#include <unordered_set>
#include <unordered_map>

#include "Global.hh"
#include "ContainerIO.hh"
#include "Symmetry.hh"
#include "SimplicialComplex.hh"

namespace topcom {

  class ces_symmetryptr_iterdata : public std::vector<const Symmetry*> {
    friend std::ostream& operator<<(std::ostream& ost, const ces_symmetryptr_iterdata& symptrvec) {
      for (ces_symmetryptr_iterdata::const_iterator iter = symptrvec.begin();
	   iter != symptrvec.end();
	   ++iter) {
	ost << **iter << " ";
      }
      return ost;
    }
  };

  // holds a symmetry container for each simplex:
  typedef std::unordered_map<Simplex, ces_symmetryptr_iterdata, Hash<Simplex> > ces_relevant_symmetryptr_table_type;

  class ClassifiedExtensionSymmetries {
  private:
    static ces_symmetryptr_iterdata emptyset;
  public:
    typedef enum { pointidx_mode, simpidx_mode }    mode_type;
  private:
    mode_type                                       _mode;
    parameter_type                                  _n;
    int                                             _no_of_workers;
    const SimplicialComplex*                        _simplicesptr;
    // a pointer to the symmetry group (not owned by this class):
    const SymmetryGroup*                            _symmetriesptr;
    // a vector of symmetry pointers to symmetries for each worker:
    std::vector<ces_symmetryptr_iterdata>            _worker_symmetryptrs;
    // entry (simp) holds all symmetries that strictly lex-decrease simp:
    ces_relevant_symmetryptr_table_type              _relevant_symmetryptrs;
    // the same distributed over a number of workers:
    std::vector<ces_relevant_symmetryptr_table_type> _worker_relevant_symmetryptrs;
  private:
    ClassifiedExtensionSymmetries();
  public:
    // constructors:
    inline ClassifiedExtensionSymmetries(const mode_type mode, // whether the symmetries act on point or simplex indices
					 const parameter_type, // no of points
					 const int); // no of workers
    inline ClassifiedExtensionSymmetries(const ClassifiedExtensionSymmetries&);
    ClassifiedExtensionSymmetries(const mode_type mode,
				  const int, // no of workers
				  const SimplicialComplex&, // all relevant simplices
				  const SymmetryGroup&); // complete symmetry group
  
    // destructor:
    inline ~ClassifiedExtensionSymmetries();

    // assignement:
    inline const ClassifiedExtensionSymmetries& operator=(const ClassifiedExtensionSymmetries&);

    // accessors:
    inline const mode_type                            mode() const;
    inline const parameter_type                       n() const;
    inline const int                                  no_of_workers() const;
    inline const SimplicialComplex&                   simplices() const;
    inline const SymmetryGroup&                       symmetries() const;
    inline const ces_symmetryptr_iterdata&            worker_symmetryptrs(const int) const;
    inline const ces_symmetryptr_iterdata&            relevant_symmetryptrs(const Simplex&) const;
    inline const ces_relevant_symmetryptr_table_type& worker_relevant_symmetryptrs(const int) const;
    inline const ces_symmetryptr_iterdata&            worker_relevant_symmetryptrs(const int,
										   const Simplex&) const;
    // functionality:
    const bool lex_decreases_by_relevant_symmetries(const SimplicialComplex&,
						    const Simplex&) const;

    // stream input/output:
    std::ostream& write_relevant_symmetry_fractions(std::ostream&) const;
    inline std::istream& read(std::istream&);
    inline friend std::istream& operator>>(std::istream&,
					   ClassifiedExtensionSymmetries&);
    inline std::ostream& write(std::ostream&) const;
    inline friend std::ostream& operator<<(std::ostream&,
					   const ClassifiedExtensionSymmetries&);  
  };

  // constructors:
  inline ClassifiedExtensionSymmetries::ClassifiedExtensionSymmetries(const mode_type      mode,
								      const parameter_type n,
								      const int            no_of_workers) :
    _mode(mode),
    _n(n),
    _no_of_workers(no_of_workers),
    _simplicesptr(0),
    _symmetriesptr(0),
    _worker_symmetryptrs(),
    _relevant_symmetryptrs(ces_relevant_symmetryptr_table_type()),
    _worker_relevant_symmetryptrs(no_of_workers, ces_relevant_symmetryptr_table_type()) {}

  inline ClassifiedExtensionSymmetries::ClassifiedExtensionSymmetries(const ClassifiedExtensionSymmetries& ces) :
    _mode(ces._mode),
    _n(ces._n),
    _no_of_workers(ces._no_of_workers),
    _simplicesptr(ces._simplicesptr),
    _symmetriesptr(ces._symmetriesptr),
    _worker_symmetryptrs(ces._worker_symmetryptrs),
    _relevant_symmetryptrs(ces._relevant_symmetryptrs),
    _worker_relevant_symmetryptrs(ces._worker_relevant_symmetryptrs) {}

  // destructor:
  inline ClassifiedExtensionSymmetries::~ClassifiedExtensionSymmetries() {
#ifdef CONSTRUCTOR_DEBUG
    std::cerr << "ClassifiedExtensionSymmetries::~ClassifiedExtensionSymmetries(): destructor called." << std::endl;
#endif
  }

  // assignement:
  inline const ClassifiedExtensionSymmetries& ClassifiedExtensionSymmetries::operator=(const ClassifiedExtensionSymmetries& ces) {
    if (&ces == this) {
      return *this;
    }
    _mode                         = ces._mode;
    _n                            = ces._n;
    _no_of_workers                = ces._no_of_workers;
    _simplicesptr                 = ces._simplicesptr;
    _symmetriesptr                = ces._symmetriesptr;
    _worker_symmetryptrs          = ces._worker_symmetryptrs;
    _relevant_symmetryptrs        = ces._relevant_symmetryptrs;
    _worker_relevant_symmetryptrs = ces._worker_relevant_symmetryptrs;
    return *this;
  }

  // accessors:
  inline const ClassifiedExtensionSymmetries::mode_type ClassifiedExtensionSymmetries::mode() const {
    return _mode;
  }

  inline const parameter_type ClassifiedExtensionSymmetries::n() const {
    return _n;
  }

  inline const int ClassifiedExtensionSymmetries::no_of_workers() const {
    return _no_of_workers;
  }

  inline const SimplicialComplex& ClassifiedExtensionSymmetries::simplices() const {
    return *_simplicesptr;
  }

  inline const SymmetryGroup& ClassifiedExtensionSymmetries::symmetries() const {
    return *_symmetriesptr;
  }

  inline const ces_symmetryptr_iterdata& ClassifiedExtensionSymmetries::worker_symmetryptrs(const int t) const {
    return _worker_symmetryptrs[t];
  }

  inline const ces_symmetryptr_iterdata& ClassifiedExtensionSymmetries::relevant_symmetryptrs(const Simplex& simp) const {
    ces_relevant_symmetryptr_table_type::const_iterator finder = _relevant_symmetryptrs.find(simp);
    if (finder != _relevant_symmetryptrs.end()) {
      return finder->second;
    }
    else {
      return emptyset;
    }
  }

  inline const ces_relevant_symmetryptr_table_type& ClassifiedExtensionSymmetries::worker_relevant_symmetryptrs(const int t) const {
    return _worker_relevant_symmetryptrs[t];
  }

  inline const ces_symmetryptr_iterdata& ClassifiedExtensionSymmetries::worker_relevant_symmetryptrs(const int      t,
												     const Simplex& simp) const {
    ces_relevant_symmetryptr_table_type::const_iterator finder = _worker_relevant_symmetryptrs[t].find(simp);
    if (finder != _worker_relevant_symmetryptrs[t].end()) {
      return finder->second;
    }
    else {
      return emptyset;
    }
  }

  // stream input/output:
  inline std::istream& ClassifiedExtensionSymmetries::read(std::istream& ist) {
    std::cerr << "ClassifiedExtensionSymmetries::read(std::istream& ist):"
	      << " reading in a pointer-based structure not supported - exiting."
	      << std::endl;
    exit(1);
    return ist;
  }

  inline std::istream& operator>>(std::istream& ist,
				  ClassifiedExtensionSymmetries& cs) {
    return cs.read(ist);
  }

  inline std::ostream& ClassifiedExtensionSymmetries::write(std::ostream& ost) const {
    const char colon = ':';
    const char comma = ',';
    const char lbracket = '[';
    const char rbracket = ']';
    ost << lbracket << _n << colon
	<< _relevant_symmetryptrs << comma
	<< _worker_relevant_symmetryptrs << rbracket;
    return ost;
  }

  inline std::ostream& operator<<(std::ostream& ost,
				  const ClassifiedExtensionSymmetries& cs) {
    return cs.write(ost);
  }

}; // namespace topcom

#endif

// eof ClassifiedExtensionSymmetries.hh
