//////////////////////////////////////////////////////////////////////////
// ClassifiedSubsetSymmetries.hh
// produced: 17/03/2020 jr
// last change: 17/03/2020 jr
/////////////////////////////////////////////////////////////////////////
#ifndef CLASSIFIEDSUBSETSYMMETRIES_HH
#define CLASSIFIEDSUBSETSYMMETRIES_HH

#include <assert.h>
#include <iostream>
#include <iterator>

#include <vector>
#include <unordered_set>

#include "Global.hh"
#include "ContainerIO.hh"
#include "Symmetry.hh"
#include "PermutationPartialComparator.hh"

namespace topcom {
  
  class css_symmetryptr_iterdata : public std::vector<const Symmetry*> {
    friend std::ostream& operator<<(std::ostream& ost, const css_symmetryptr_iterdata symptrvec) {
      for (css_symmetryptr_iterdata::const_iterator iter = symptrvec.begin();
	   iter != symptrvec.end();
	   ++iter) {
	ost << **iter << " ";
      }
      return ost;
    }
  };

  typedef std::unordered_set<PermutationPartialComparator,
			     Hash<PermutationPartialComparator> >      css_symmetryptr_collectordata;

  typedef std::vector<std::vector<css_symmetryptr_iterdata> >           css_relevant_symmetryptr_table_type;
  typedef std::vector<css_symmetryptr_collectordata>                    css_relevant_symmetryptr_collector_type;

  typedef std::vector<std::vector<bool> >                              css_overtaking_symmetry_existence_type;
  typedef std::vector<std::vector<std::vector<bool> > >                css_compressing_symmetry_existence_type;

  class ClassifiedSubsetSymmetries {
  private:
    parameter_type                                  _n;
    parameter_type                                  _no_of_tables;
    int                                             _no_of_workers;
    // a pointer to the symmetry group (not owned by this class):
    const SymmetryGroup*                            _symmetriesptr;
    // a vector of symmetry pointers to symmetries for each worker:
    std::vector<css_symmetryptr_iterdata>            _worker_symmetryptrs;
    // entry (i, j) holds all symmetries that differ on values between i and j:
    css_relevant_symmetryptr_table_type              _relevant_symmetryptrs;
    // the same distributed over a number of workers:
    std::vector<css_relevant_symmetryptr_table_type> _worker_relevant_symmetryptrs;
    // entry (i, j) is true iff
    // there is a symmetry g with g(i) > j:
    css_overtaking_symmetry_existence_type           _overtaking_symmetry_exists;
    // entry (i, j, k) is true iff
    // there is a symmetry g with g(i) > j and g(l) = l for all l >= k:
    css_compressing_symmetry_existence_type          _compressing_symmetry_exists;
    std::vector<int>                                 _index_maxelem_table;
    std::vector<int>                                 _maxelem_index_table;
  private:
    ClassifiedSubsetSymmetries();
  public:
    // constructors:
    inline ClassifiedSubsetSymmetries(const parameter_type,
				      const parameter_type,
				      const int);
    inline ClassifiedSubsetSymmetries(const ClassifiedSubsetSymmetries&);
    ClassifiedSubsetSymmetries(const parameter_type,
			       const int,
			       const SymmetryGroup&);

    // destructor:
    inline ~ClassifiedSubsetSymmetries();

    // assignement:
    inline const ClassifiedSubsetSymmetries& operator=(const ClassifiedSubsetSymmetries&);

    // accessors:
    inline const parameter_type                       n() const;
    inline const parameter_type                       no_of_tables() const;
    inline const int                                  no_of_workers() const;
    inline const SymmetryGroup&                       symmetries() const;
    inline const css_symmetryptr_iterdata&            worker_symmetryptrs(const int) const;
    inline const css_symmetryptr_iterdata&            relevant_symmetryptrs(const parameter_type,
									    const parameter_type) const;
    inline const css_relevant_symmetryptr_table_type& worker_relevant_symmetryptrs(const int) const;
    inline const css_symmetryptr_iterdata&            worker_relevant_symmetryptrs(const int            t,
										   const parameter_type i,
										   const parameter_type j) const;
    inline const bool                                 overtaking_symmetry_exists(const parameter_type,
										 const parameter_type) const;
    inline const bool                                 compressing_symmetry_exists(const parameter_type,
										  const parameter_type,
										  const parameter_type) const;

    inline const parameter_type                       max_elem(const parameter_type) const;
    inline const parameter_type                       relevant_index(const parameter_type max) const;
    // manipulators:
    inline void clear_relevant_symmetryptrs(const parameter_type,
					    const parameter_type);

    // functionality:
    const bool colex_increases_by_overtaking(const LabelSet&,
					     const parameter_type,
					     const parameter_type) const;
    const bool colex_increases_by_compressing(const LabelSet&,
					      const parameter_type,
					      const parameter_type) const;  
    const bool colex_increases_by_relevant_symmetries(const LabelSet&,
						      const parameter_type,
						      const parameter_type) const;

    // stream input/output:
    std::ostream& write_relevant_symmetry_fractions(std::ostream&) const;
    inline std::istream& read(std::istream&);
    inline friend std::istream& operator>>(std::istream&,
					   ClassifiedSubsetSymmetries&);
    inline std::ostream& write(std::ostream&) const;
    inline friend std::ostream& operator<<(std::ostream&,
					   const ClassifiedSubsetSymmetries&);  
  };

  // constructors:
  inline ClassifiedSubsetSymmetries::ClassifiedSubsetSymmetries(const parameter_type n,
								const parameter_type no_of_tables,
								const int            no_of_workers) :
    _n(n),
    _no_of_tables(no_of_tables),
    _no_of_workers(no_of_workers),
    _symmetriesptr(0),
    _worker_symmetryptrs(),
    _relevant_symmetryptrs(css_relevant_symmetryptr_table_type(n, std::vector<css_symmetryptr_iterdata>(no_of_tables, css_symmetryptr_iterdata()))),
    _worker_relevant_symmetryptrs(no_of_workers, css_relevant_symmetryptr_table_type(n, std::vector<css_symmetryptr_iterdata>(no_of_tables, css_symmetryptr_iterdata()))),
    _overtaking_symmetry_exists(css_overtaking_symmetry_existence_type(n, std::vector<bool>(n, false))),
    _compressing_symmetry_exists(css_compressing_symmetry_existence_type(n, std::vector<std::vector<bool> >(n, std::vector<bool>(n, false)))),
    _index_maxelem_table(no_of_tables),
    _maxelem_index_table(n) {
    for (int i = 0; i < _no_of_tables; ++i) {
      _index_maxelem_table[i] = max_elem(i);
    }
    for (int k = 0; k < _n; ++k) {
      _maxelem_index_table[k] = relevant_index(k);
    }
  }

  inline ClassifiedSubsetSymmetries::ClassifiedSubsetSymmetries(const ClassifiedSubsetSymmetries& cs) :
    _n(cs._n),
    _no_of_tables(cs._no_of_tables),
    _symmetriesptr(cs._symmetriesptr),
    _worker_symmetryptrs(cs._worker_symmetryptrs),
    _relevant_symmetryptrs(cs._relevant_symmetryptrs),
    _worker_relevant_symmetryptrs(cs._worker_relevant_symmetryptrs),
    _overtaking_symmetry_exists(cs._overtaking_symmetry_exists),
    _compressing_symmetry_exists(cs._compressing_symmetry_exists),
    _index_maxelem_table(cs._index_maxelem_table),
    _maxelem_index_table(cs._maxelem_index_table) {}

  // destructor:
  inline ClassifiedSubsetSymmetries::~ClassifiedSubsetSymmetries() {}

  // assignement:
  inline const ClassifiedSubsetSymmetries& ClassifiedSubsetSymmetries::operator=(const ClassifiedSubsetSymmetries& cs) {
    if (&cs == this) {
      return *this;
    }
    _n                           = cs._n;
    _no_of_tables                = cs._no_of_tables;
    _symmetriesptr               = cs._symmetriesptr;
    _worker_symmetryptrs         = cs._worker_symmetryptrs;
    _relevant_symmetryptrs       = cs._relevant_symmetryptrs;
    _overtaking_symmetry_exists  = cs._overtaking_symmetry_exists;
    _compressing_symmetry_exists = cs._compressing_symmetry_exists;
    _index_maxelem_table         = cs._index_maxelem_table;
    _maxelem_index_table         = cs._maxelem_index_table;
    return *this;
  }

  // accessors:
  inline const parameter_type ClassifiedSubsetSymmetries::n() const {
    return _n;
  }

  inline const parameter_type ClassifiedSubsetSymmetries::no_of_tables() const {
    return _no_of_tables;
  }

  inline const int ClassifiedSubsetSymmetries::no_of_workers() const {
    return _no_of_workers;
  }

  inline const SymmetryGroup& ClassifiedSubsetSymmetries::symmetries() const {
    return *_symmetriesptr;
  }

  inline const css_symmetryptr_iterdata& ClassifiedSubsetSymmetries::worker_symmetryptrs(const int t) const {
    return _worker_symmetryptrs[t];
  }

  inline const css_symmetryptr_iterdata& ClassifiedSubsetSymmetries::relevant_symmetryptrs(const parameter_type i,
											   const parameter_type j) const {
    return _relevant_symmetryptrs[i][_maxelem_index_table[j]];
  }

  inline const css_relevant_symmetryptr_table_type& ClassifiedSubsetSymmetries::worker_relevant_symmetryptrs(const int t) const {
    return _worker_relevant_symmetryptrs[t];
  }

  inline const css_symmetryptr_iterdata& ClassifiedSubsetSymmetries::worker_relevant_symmetryptrs(const int            t,
												  const parameter_type i,
												  const parameter_type j) const {
    return _worker_relevant_symmetryptrs[t][i][_maxelem_index_table[j]];
  }

  inline const bool ClassifiedSubsetSymmetries::overtaking_symmetry_exists(const parameter_type i,
									   const parameter_type j) const {
    return _overtaking_symmetry_exists[i][j];
  }
  inline const bool ClassifiedSubsetSymmetries::compressing_symmetry_exists(const parameter_type i,
									    const parameter_type j,
									    const parameter_type k) const {
    return _compressing_symmetry_exists[i][j][k];
  }

  inline const parameter_type ClassifiedSubsetSymmetries::max_elem(const parameter_type j) const {
    return ((_no_of_tables - j) * (_n - 1)) / _no_of_tables;
  }

  inline const parameter_type ClassifiedSubsetSymmetries::relevant_index(const parameter_type max) const {
    parameter_type k = _no_of_tables - 1;
    while ((k >= 0) && (max_elem(k) < max)) {
      --k;
    }
#ifdef DEBUG
    std::cerr << "relevant index of " << max << " is " << k << std::endl;
#endif
    return k;
  }

  // manipulators:
  inline void ClassifiedSubsetSymmetries::clear_relevant_symmetryptrs(const parameter_type i,
								      const parameter_type j) {
    _relevant_symmetryptrs[i][j].clear();
  }


  // stream input/output:
  inline std::istream& ClassifiedSubsetSymmetries::read(std::istream& ist) {
    std::cerr << "ClassifiedSubsetSymmetries::read(std::istream& ist):"
	      << " reading in a pointer-based structure not supported - exiting."
	      << std::endl;
    exit(1);
    return ist;
  }

  inline std::istream& operator>>(std::istream& ist,
				  ClassifiedSubsetSymmetries& cs) {
    return cs.read(ist);
  }

  inline std::ostream& ClassifiedSubsetSymmetries::write(std::ostream& ost) const {
    const char colon = ':';
    const char comma = ',';
    const char lbracket = '[';
    const char rbracket = ']';
    ost << lbracket << _n << colon
	<< _relevant_symmetryptrs << comma
	<< _overtaking_symmetry_exists << comma
	<< _compressing_symmetry_exists << rbracket;
    return ost;
  }

  inline std::ostream& operator<<(std::ostream& ost,
				  const ClassifiedSubsetSymmetries& cs) {
    return cs.write(ost);
  }

}; // namespace topcom

#endif

// eof ClassifiedSubsetSymmetries.hh
