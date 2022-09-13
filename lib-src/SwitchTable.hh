//////////////////////////////////////////////////////////////////////////
// SwitchTable.hh
// produced: 09/03/2020 jr
// last change: 09/03/2020 jr
// (based on "Parallel Enumeration of Triangulations" by
// Jordan, Joswig, and Kastner, 2018)
/////////////////////////////////////////////////////////////////////////
#ifndef SWITCHTABLE_HH
#define SWITCHTABLE_HH

#include <assert.h>
#include <iostream>
#include <iterator>

#include <vector>

#include "Global.hh"
#include "ContainerIO.hh"
#include "LabelSet.hh"
#include "Symmetry.hh"

namespace topcom {

  class switch_table_row_type : public std::vector<Symmetry> {
    friend std::ostream& operator<<(std::ostream& ost, const switch_table_row_type str) {
      for (switch_table_row_type::const_iterator iter = str.begin();
	   iter != str.end();
	   ++iter) {
	ost << *iter << " ";
      }
      return ost;
    }
  public:
    inline switch_table_row_type() :
      std::vector<Symmetry>() {}
    inline switch_table_row_type(const parameter_type n) :
      std::vector<Symmetry>(n, Symmetry(0)) {}
  };

  class switch_selection_type : public std::vector<const Symmetry*> {
    friend std::ostream& operator<<(std::ostream& ost, const switch_selection_type ss) {
      for (switch_selection_type::const_iterator iter = ss.begin();
	   iter != ss.end();
	   ++iter) {
	ost << **iter << " ";
      }
      return ost;
    }
  public:
    inline switch_selection_type() :
      std::vector<const Symmetry*>() {}
  };

  typedef std::vector<switch_table_row_type> switch_table_data_type;

  template <comparison_mode_type mode>
  class SwitchTable : public switch_table_data_type {
  public:
    typedef comparison_mode_type mode_type;
  private:
    parameter_type                _n;
    LabelSet                      _non_trivial_rowset;
    std::vector<LabelSet>         _non_trivial_colsetvec;
    Symmetry                      _identity;
    size_type                     _order;
  private:
    SwitchTable();
  public:
    // constructors:
    SwitchTable(const parameter_type); // contains only the identity
    SwitchTable(const parameter_type,
		const symmetry_collectordata&); // built from generators
    SwitchTable(const SymmetryGroup&); // built from symmetry group
    inline SwitchTable(const SwitchTable&); // copied
  
    // destructor:
    inline ~SwitchTable();
  
    // assignment:
    inline SwitchTable& operator=(const SwitchTable&);

    // accessors:
    inline parameter_type                     n() const;
    inline const LabelSet&                    non_trivial_rowset() const;
    inline const std::vector<LabelSet>&       non_trivial_colsetvec() const;
    inline const Symmetry&                    identity() const;
    inline size_type                          order() const;
    // functionality:
    // standard functions:
    bool                                empty          () const;
    size_type                           compute_order  () const;
    bool                                is_in_group    (const Symmetry&) const;
    bool                                add_to_group   (const Symmetry&);
    size_type                           stabilizer_card(const IntegerSet&) const;
    size_type                           stabilizer_card(const IntegerSet64&) const;
    size_type                           orbit_size     (const IntegerSet&) const;
    size_type                           orbit_size     (const IntegerSet64&) const;
    symmetry_collectordata              all_switches   () const;
    symmetry_collectordata              all_switches   (const parameter_type) const;

    // extract those switches that may colex-increase a LabelSet:
    switch_selection_type               canonicalizing_switchptrs(const parameter_type,
								  const LabelSet&) const;
    switch_selection_type               neutral_switchptrs(const parameter_type i,
							   const LabelSet& ls) const;
    // Jordan-Joswig-Kastner specialization:
    bool                                not_canonical  (const parameter_type,
							const LabelSet&,
							const LabelSet&) const;
    // combination of Jordan-Joswig-Kastner with Pech-Reichard:
    bool                                not_canonicalPR(const parameter_type,
							const LabelSet&,
							const LabelSet&) const;
    bool                                lex_decreases  (const LabelSet&) const;
    bool                                colex_increases(const LabelSet&) const;

    // scan switch products stabilizing a subset:
    switch_selection_type               stabilizing_switchptrs(const parameter_type,
							       const LabelSet&,
							       const LabelSet&) const;
    size_type                           count_stabilizing_switchproducts(const parameter_type,
									 const LabelSet&,
									 const LabelSet&) const;
   
    // stream input/output:
    inline std::istream& read(std::istream&);
    template <mode_type streammode>
    inline friend std::istream& operator>>(std::istream&, SwitchTable<streammode>&);
    inline std::ostream& write(std::ostream&) const;
    template <mode_type streammode>
    inline friend std::ostream& operator<<(std::ostream&, const SwitchTable<streammode>&);

  private:
    
    // internal methods:
    inline parameter_type _elem(const parameter_type) const;
    inline parameter_type _mincol(const parameter_type) const;
    inline parameter_type _maxcol(const parameter_type) const;
    inline bool _compare_subsets(const LabelSet&, const LabelSet&) const;
    bool _add_symmetry(const Symmetry&);
  };
  
  // forward template specializations:
  template <>
  inline parameter_type SwitchTable<lexmin_mode>::_elem(const parameter_type) const;
  
  template <>
  inline parameter_type SwitchTable<colexmax_mode>::_elem(const parameter_type) const;

  template <>
  inline parameter_type SwitchTable<lexmin_mode>::_mincol(const parameter_type) const;
  
  template <>
  inline parameter_type SwitchTable<colexmax_mode>::_mincol(const parameter_type) const;

  template <>
  inline parameter_type SwitchTable<lexmin_mode>::_maxcol(const parameter_type) const;
  
  template <>
  inline parameter_type SwitchTable<colexmax_mode>::_maxcol(const parameter_type) const;

  template <>
  inline bool SwitchTable<lexmin_mode>::_compare_subsets(const LabelSet&, const LabelSet&) const;
  
  template <>
  inline bool SwitchTable<colexmax_mode>::_compare_subsets(const LabelSet&, const LabelSet&) const;

  // constructors:
  template <comparison_mode_type mode>
  inline SwitchTable<mode>::SwitchTable(const SwitchTable& st) :
    switch_table_data_type(st),
    _n(st._n),
    _non_trivial_rowset(st._non_trivial_rowset),
    _non_trivial_colsetvec(st._non_trivial_colsetvec),
    _identity(st._identity),
    _order(st._order) {
  }
  
  template <comparison_mode_type mode>
  SwitchTable<mode>::SwitchTable(const parameter_type n) :
    switch_table_data_type(switch_table_data_type(n, switch_table_row_type(n))),
    _n(n),
    _non_trivial_rowset(),
    _non_trivial_colsetvec(n, LabelSet()),
    _identity(Symmetry(n)),
    _order(1UL) {
  }
     
  template <comparison_mode_type mode>
  SwitchTable<mode>::SwitchTable(const parameter_type n,
				 const symmetry_collectordata& generators) :
    switch_table_data_type(switch_table_data_type(n, switch_table_row_type(n))),
    _n(n),
    _non_trivial_rowset(),
    _non_trivial_colsetvec(n, LabelSet()),
    _identity(Symmetry(n)),
    _order(1UL) {
    if (CommandlineOptions::verbose()) {
      std::cerr << "building switch table ..." << std::endl;
    }

    bool new_entries_found = false;
    
    // replace the previous entry wherever a lex-decreasing/colex-increasing switch exists:
    for (symmetry_collectordata::const_iterator geniter = generators.begin();
	 geniter != generators.end();
	 ++geniter) {
      if (_add_symmetry(*geniter)) {
	new_entries_found = true;
      }
    }
    
    while (new_entries_found) {
      new_entries_found = false;
      
      // replace the previous entry wherever a lex-decreasing/colex-increasing switch exists:
      for (symmetry_collectordata::const_iterator geniter = generators.begin();
	   geniter != generators.end();
	   ++geniter) {
	const Symmetry& generator(*geniter);
	for (LabelSet::const_iterator rowiter = _non_trivial_rowset.begin();
	     rowiter != _non_trivial_rowset.end();
	     ++rowiter) {
	  for (LabelSet::const_iterator coliter = _non_trivial_colsetvec.at(*rowiter).begin();
	       coliter != _non_trivial_colsetvec.at(*rowiter).end();
	       ++coliter) {
	    const Symmetry sym = this->at(*rowiter).at(*coliter) * generator;
	    if (add_to_group(sym)) {
	      new_entries_found = true;
	    }
	  }
	}
      }
    }
    _order = compute_order();
    if (CommandlineOptions::debug()) {

      // check whether each symmetry in the symmetry group is a product of switches
      // of the constructed switch table:

      SymmetryGroup sg(n, generators, true);
      for (SymmetryGroup::const_iterator symiter = sg.begin();
	   symiter != sg.end();
	   ++symiter) {
	if (!this->is_in_group(*symiter)) {
	  std::cerr << "SwitchTable::SwitchTable(const mode_type mode, const parameter_type& n): "
		    << "error - symmetry " << *symiter
		    << " not a product of switches in\n" << *this
		    << " - exiting" << std::endl;
	  exit(1);
	}
      }
    }
    if (CommandlineOptions::verbose()) {
      if (empty()) {
	std::cerr << "switch table has no rows" << std::endl;
      }
      else {
	std::cerr << "switch table has " << _non_trivial_rowset.max_elem()
		  << " rows for a symmetry group of order " << _order
		  << " and degree " << _n << std::endl;
      }
      std::cerr << "... done" << std::endl;
    }
  }
   
  template <comparison_mode_type mode>
  SwitchTable<mode>::SwitchTable(const SymmetryGroup& sg) :
    switch_table_data_type(switch_table_data_type(sg.n(), switch_table_row_type(sg.n()))),
    _n(sg.n()),
    _non_trivial_rowset(),
    _non_trivial_colsetvec(sg.n(), LabelSet()),
    _identity(sg.identity()),
    _order(1UL) {
    if (CommandlineOptions::verbose()) {
      std::cerr << "building switch table ..." << std::endl;
    }

    if (!sg.complete()) {
      std::cerr << "SwitchTable::SwitchTable(const mode_type mode, const SymmetryGroup& sg): "
		<< "symmetry group must be complete for switch table construction - exiting" << std::endl;
      exit(1);
    }
    
    // replace the previous entry wherever a lex-decreasing/colex-increasing switch exists:
    for (SymmetryGroup::const_iterator iter = sg.begin();
	 iter != sg.end();
	 ++iter) {
      _add_symmetry(*iter);
    }
    if (CommandlineOptions::debug()) {
      
      // check whether each symmetry in the symmetry group is a product of switches
      // of the constructed switch table:
      for (SymmetryGroup::const_iterator symiter = sg.begin();
	   symiter != sg.end();
	   ++symiter) {
	if (!this->is_in_group(*symiter)) {
	  std::cerr << "SwitchTable::SwitchTable(const mode_type mode, const parameter_type& n): "
		    << "error - symmetry " << *symiter
		    << " not a product of switches in " << *this
		    << " - exiting" << std::endl;
	  exit(1);
	}
      }
    }
    _order = compute_order();
    if (CommandlineOptions::verbose()) {
      if (empty()) {
	std::cerr << "switch table has no rows" << std::endl;
      }
      else {
	std::cerr << "switch table has " << _non_trivial_rowset.max_elem()
		  << " rows for a symmetry group of order " << _order << std::endl;
      }
      std::cerr << "... done" << std::endl;
    }
  }

  // destructor:
  template <comparison_mode_type mode>
  inline SwitchTable<mode>::~SwitchTable() {
  }

  // assignment:
  template <comparison_mode_type mode>
  inline SwitchTable<mode>& SwitchTable<mode>::operator=(const SwitchTable& st) {
    if (&st == this) {
      return *this;
    }
    switch_table_data_type::operator=(st);
    _n = st._n;
    _non_trivial_rowset = st._non_trivial_rowset;
    _non_trivial_colsetvec = st._non_trivial_colsetvec;
    _identity = st._identity;
    _order = st._order;
    return *this;
  }

  // accessors:
  template <comparison_mode_type mode>
  inline parameter_type SwitchTable<mode>::n() const {
    return _n;
  }

  template <comparison_mode_type mode>
  inline const LabelSet& SwitchTable<mode>::non_trivial_rowset() const {
    return _non_trivial_rowset;
  }

  template <comparison_mode_type mode>
  inline const std::vector<LabelSet>& SwitchTable<mode>::non_trivial_colsetvec() const {
    return _non_trivial_colsetvec;
  }

  template <comparison_mode_type mode>
  inline const Symmetry& SwitchTable<mode>::identity() const {
    return _identity;
  }

  template <comparison_mode_type mode>
  inline size_type SwitchTable<mode>::order() const {
    return _order;
  }

  // standard functions:
  template <comparison_mode_type mode>
  bool SwitchTable<mode>::empty() const {
    return _non_trivial_rowset.empty();
  }

  template <comparison_mode_type mode>
  size_type SwitchTable<mode>::compute_order() const {
    size_type result(1UL);
    for (LabelSet::const_iterator rowiter = _non_trivial_rowset.begin();
	 rowiter != _non_trivial_rowset.end();
	 ++rowiter) {
      result *= (_non_trivial_colsetvec.at(*rowiter).card() + 1);
    }
    return result;
  }

  template <comparison_mode_type mode>
  bool SwitchTable<mode>::is_in_group(const Symmetry& s) const {
    Symmetry product(s);
    // std::vector<Symmetry> word(_n, Symmetry(_n));
    if (CommandlineOptions::debug()) {
      std::cerr << "initial permutation : " << product << std::endl;
      // std::cerr << "initial trivial word: " << word << std::endl;
    }
    for (parameter_type i = 0; i < _n; ++i) {
      const parameter_type elem(_elem(i));
      if (product(elem) != elem) {
	for (parameter_type j = _mincol(elem); j < _maxcol(elem); ++j) {
	  if (product(j) == elem) {
	    if (!_non_trivial_colsetvec.at(i).contains(j)) {
	      if (CommandlineOptions::debug()) {
		std::cerr << "current permutation : " << product << std::endl;
		std::cerr << "switch table        :\n" << *this << std::endl;
	      }
	      return false;
	    }
	    else {
	      // word.at(i) = this->at(i).at(j);
	      if (CommandlineOptions::debug()) {
		std::cerr << "current permutation : " << this->at(i).at(j).inverse() * product
			  << " = " << this->at(i).at(j).inverse()
			  << " * " << product << std::endl;
		// std::cerr << "current word        : " << word << std::endl;
	      }
	      product = product * this->at(i).at(j).inverse();
	    }
	  }
	}
      }
    }
    if (CommandlineOptions::debug()) {
      std::cerr << "final permutation   : " << product << std::endl;
      // std::cerr << "final word          : " << word << std::endl;
    }
    return true;
  }
 
  template <comparison_mode_type mode>
  bool SwitchTable<mode>::add_to_group(const Symmetry& s) {
    Symmetry product(s);
    // std::vector<Symmetry> word(_n, Symmetry(_n));
    if (CommandlineOptions::debug()) {
      std::cerr << "initial permutation : " << product << std::endl;
      // std::cerr << "initial trivial word: " << word << std::endl;
    }
    for (parameter_type i = 0; i < _n; ++i) {
      const parameter_type elem(_elem(i));
      if (product(elem) != elem) {
	for (parameter_type j = _mincol(elem); j < _maxcol(elem); ++j) {
	  if (product(j) == elem) {
	    if (!_non_trivial_colsetvec.at(i).contains(j)) {
	      this->_non_trivial_rowset += i;
	      this->_non_trivial_colsetvec.at(i) += j;
	      this->at(i).at(j) = product;
	      return true;
	    }
	    else {
	      // word.at(i) = this->at(i).at(j);
	      product = product * this->at(i).at(j).inverse();
	      if (CommandlineOptions::debug()) {
		std::cerr << "current permutation : " << product << std::endl;
		// std::cerr << "current word        : " << word << std::endl;
	      }
	    }
	  }
	}
      }
    }
    if (CommandlineOptions::debug()) {
      std::cerr << "final permutation   : " << product << std::endl;
      // std::cerr << "final word          : " << word << std::endl;
    }
    return false;
  }

  template <comparison_mode_type mode>
  size_type SwitchTable<mode>::stabilizer_card(const IntegerSet& is) const {
    return count_stabilizing_switchproducts(0, is, is);
  }
  
  template <comparison_mode_type mode>
  size_type SwitchTable<mode>::stabilizer_card(const IntegerSet64& is) const {
    return count_stabilizing_switchproducts(0, is, is);
  }

  template <comparison_mode_type mode>
  size_type SwitchTable<mode>::orbit_size(const IntegerSet& is) const {
    return _order / stabilizer_card(is);
  }

  template <comparison_mode_type mode>
  size_type SwitchTable<mode>::orbit_size(const IntegerSet64& is) const {
    return _order / stabilizer_card(is);
  }

  template <comparison_mode_type mode>
  symmetry_collectordata SwitchTable<mode>::all_switches(const parameter_type i) const {
    symmetry_collectordata result;
    for (LabelSet::const_iterator coliter = _non_trivial_colsetvec.at(i).begin();
	 coliter != _non_trivial_colsetvec.at(i).end();
	 ++coliter) {
      result.insert(this->at(i).at(*coliter));
    }
    return result;
  }

  template <comparison_mode_type mode>
  symmetry_collectordata SwitchTable<mode>::all_switches() const {
    symmetry_collectordata result;
    for (LabelSet::const_iterator rowiter = _non_trivial_rowset.begin();
	 rowiter != _non_trivial_rowset.end();
	 ++rowiter) {
      for (LabelSet::const_iterator coliter = _non_trivial_colsetvec.at(*rowiter).begin();
	   coliter != _non_trivial_colsetvec.at(*rowiter).end();
	   ++coliter) {
	result.insert(this->at(*rowiter).at(*coliter));
      }
    }
    return result;
  }

  // extract those switches that may lex-decrease/colex-increase a LabelSet:
  template <comparison_mode_type mode>
  switch_selection_type SwitchTable<mode>::canonicalizing_switchptrs(const parameter_type i,
								     const LabelSet& ls) const {

    // these switches necessarily canonicalize ls strictly:
    switch_selection_type result;
    result.reserve(_non_trivial_colsetvec.at(i).card());
    const parameter_type elem(_elem(i));
    if (_non_trivial_rowset.contains(i)) {
      if (!ls.contains(elem)) {
	const LabelSet canonicalizing_colset = _non_trivial_colsetvec.at(i) * ls;
	for (LabelSet::const_iterator coliter = canonicalizing_colset.begin();
	     coliter != canonicalizing_colset.end();
	     ++coliter) {
	  const parameter_type j(*coliter);
	  result.emplace_back(&this->at(i).at(j));
	}
      }
    }
    return result;
  }

  template <comparison_mode_type mode>
  switch_selection_type SwitchTable<mode>::neutral_switchptrs(const parameter_type i,
							      const LabelSet& ls) const {

    // these switches behave neutrally w.r.t. ls:
    switch_selection_type result;
    result.reserve(_non_trivial_colsetvec.at(i).card());
    const parameter_type elem(_elem(i));
    if (_non_trivial_rowset.contains(i)) {
      for (LabelSet::const_iterator coliter = _non_trivial_colsetvec.at(i).begin();
	   coliter != _non_trivial_colsetvec.at(i).end();
	   ++coliter) {
	const parameter_type j(*coliter);
	if ((ls.contains(elem) && ls.contains(j))
	    || (!ls.contains(elem) && !ls.contains(j))) {
	  result.emplace_back(&this->at(i).at(j));
	}
      }
    }
    return result;
  }

  template <comparison_mode_type mode>
  bool SwitchTable<mode>::not_canonical(const parameter_type i,
					const LabelSet&      ls_org,
					const LabelSet&      ls_img) const {
    if (empty()) {
      return false;
    }
    if (i > _non_trivial_rowset.max_elem()) {
      return false;
    }

    // handle the identity individually without bogus mapping:
    if (not_canonical(i + 1, ls_org, ls_img)) {
      return true;
    }
    switch_selection_type canonicalizers(canonicalizing_switchptrs(i, ls_img));
    if (canonicalizers.empty()) {
      switch_selection_type neutrals(neutral_switchptrs(i, ls_img));
      for (switch_selection_type::const_iterator iter = neutrals.begin();
	   iter != neutrals.end();
	   ++iter) {
	const LabelSet new_ls_img(ls_img.permute(**iter));
	if (_compare_subsets(new_ls_img, ls_org)) {
	  return true;
	}
	if (not_canonical(i + 1, ls_org, new_ls_img)) {
	  return true;
	}
      }
    }
    else {
      for (switch_selection_type::const_iterator iter = canonicalizers.begin();
	   iter != canonicalizers.end();
	   ++iter) {
	const LabelSet new_ls_img(ls_img.permute(**iter));
	if (_compare_subsets(new_ls_img, ls_org)) {
	  return true;
	}
	if (not_canonical(i + 1, ls_org, new_ls_img)) {
	  return true;
	}
      }
    }
    return false;
  }

  template <comparison_mode_type mode>
  bool SwitchTable<mode>::not_canonicalPR(const parameter_type i,
					  const LabelSet& ls_org,
					  const LabelSet& ls_img) const {

    // a combination of the Pech-Reichard-algorithm with the power of switch tables:
    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "does G[" << i << "] map " << ls_img
		<< " to a better set than original set " << ls_org << "?" << std::endl;
    }
    
    if (ls_org.empty()) {
      if (CommandlineOptions::debug()) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "original set " << ls_org << " empty - false" << std::endl;
      }
      return false;
    }
    else {
      if (CommandlineOptions::debug()) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "original set " << ls_org << " non-empty - process" << std::endl;
      }      
    }

    if (empty()) {
      if (CommandlineOptions::debug()) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "group is trivial - compare argument sets" << std::endl;
      }      
      return _compare_subsets(ls_img, ls_org);
    }

    if (i > _non_trivial_rowset.max_elem()) {
      if (CommandlineOptions::debug()) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "group traversal at level " << i
		  << " is trivial - compare argument sets" << std::endl;
      }      
      return _compare_subsets(ls_img, ls_org);
    }
    
    const parameter_type elem(_elem(i));
    if (!ls_org.contains(elem)) {
      if (ls_img.contains(elem)) {
	if (CommandlineOptions::debug()) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << "original set " << ls_org << " does not contain " << elem << std::endl;
	  std::cerr << "mapped set   " << ls_img << " contains         " << elem << std::endl;
	  std::cerr << "identity maps set to a better set than original set - true" << std::endl;
	}
	return true;
      }
      else if (_non_trivial_rowset.contains(i)
	       && _non_trivial_colsetvec.at(i).intersection_nonempty(ls_img)) {
	if (CommandlineOptions::debug()) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << "original set " << ls_org << " does not contain " << elem << std::endl;
	  std::cerr << "mapped set   " << ls_img << " does not contain " << elem << std::endl;
	  std::cerr << "exist switches with j in " << _non_trivial_colsetvec.at(i) * ls_img
		    << " mapping elements of mapped set " << ls_img
		    << " to " << elem << " " << std::endl;
	  std::cerr << "G[" << i << "] maps mapped set to a better set than original set - true" << std::endl;
	}
	return true;  
      }
      else {
	if (CommandlineOptions::debug()) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << "original set " << ls_org << " does not contain " << elem << std::endl;
	  std::cerr << "mapped set   " << ls_img << " does not contain " << elem << std::endl;
	  std::cerr << "recurse:" << std::endl;
	}
	  
	// start implicitly with the identity switch:
	if (not_canonicalPR(i + 1, ls_org, ls_img)) {
	  if (CommandlineOptions::debug()) {
	    std::lock_guard<std::mutex> lock(IO_sync::mutex);
	    std::cerr << "G[" << i + 1 << "] maps " << ls_img
		      << " to a better set than " << ls_org << " - true" << std::endl;
	  }
	  return true;
	}

	// now process all neutral non-identity switches mapping non-elements to a non-element:
	for (LabelSet::const_iterator preimg_iter = _non_trivial_colsetvec.at(i).begin();
	     preimg_iter != _non_trivial_colsetvec.at(i).end();
	     ++preimg_iter) {
	  const parameter_type j(*preimg_iter);
	  if (ls_img.contains(j)) {
	    continue;
	  }
	  const LabelSet new_ls_img(ls_img.permute(this->at(i).at(j)));
	  if (not_canonicalPR(i + 1, ls_org, new_ls_img)) {
	    if (CommandlineOptions::debug()) {
	      std::lock_guard<std::mutex> lock(IO_sync::mutex);
	      std::cerr << "original set " << ls_org << " does not contain " << elem << std::endl;
	      std::cerr << "mapped set   " << ls_img << " does not contain " << elem << std::endl;
	      std::cerr << "G[" << i + 1 << "] maps " << new_ls_img
			<< " to a better set than " << ls_org << " - true" << std::endl;
	    }
	    return true;
	  }
	}
	if (CommandlineOptions::debug()) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << "original set " << ls_org << " does not contain " << elem << std::endl;
	  std::cerr << "mapped set   " << ls_img << " does not contain " << elem << std::endl;
	  std::cerr << "G[" << i + 1 << "] does not map " << ls_img
		    << " to a better set than " << ls_org << " - false" << std::endl;
	}
	return false;
      }
    }
    else {
      if (!ls_img.contains(elem)) {
	// const LabelSet possible_preimages(_non_trivial_colsetvec.at(i) * ls_img);
	LabelSet new_ls_org(ls_org - elem);
	for (LabelSet::const_iterator preimg_iter = _non_trivial_colsetvec.at(i).begin();
	     preimg_iter != _non_trivial_colsetvec.at(i).end();
	     ++preimg_iter) {
	  const parameter_type j(*preimg_iter);
	  if (!ls_img.contains(j)) {

	    // we first have to make sure that ls_img hits
	    // the current extremal element elem of ls_org:
	    continue;
	  }
	  const LabelSet new_ls_img((ls_img - j).permute(this->at(i).at(j)));
	  if (not_canonicalPR(i + 1, new_ls_org, new_ls_img)) {
	    if (CommandlineOptions::debug()) {
	      std::lock_guard<std::mutex> lock(IO_sync::mutex);
	      std::cerr << "original set " << ls_org << " contains         " << elem << std::endl;
	      std::cerr << "mapped set   " << ls_img << " does not contain " << elem << std::endl;
	      std::cerr << "G[" << i + 1 << "] maps " << new_ls_img
			<< " to a better set than " << new_ls_org << " - true" << std::endl;
	    }
	    return true;
	  }
	}
	if (CommandlineOptions::debug()) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << "original set " << ls_org << " contains         " << elem << std::endl;
	  std::cerr << "mapped set   " << ls_img << " does not contain " << elem << std::endl;
	  std::cerr << "G[" << i + 1 << "] does not map " << ls_img
		    << " to a better set than " << ls_org << " - false" << std::endl;
	}
	return false;
      }
      else {
	if (CommandlineOptions::debug()) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << "original set " << ls_org << " contains " << elem << std::endl;
	  std::cerr << "mapped set   " << ls_img << " contains " << elem << std::endl;
	  std::cerr << "recurse:" << std::endl;
	}
	
	// start implicitly with the identity switch:
	const LabelSet new_ls_img(ls_img - elem);
	LabelSet new_ls_org(ls_org - elem);
	if (not_canonicalPR(i + 1, new_ls_org, new_ls_img)) {
	  if (CommandlineOptions::debug()) {
	    std::lock_guard<std::mutex> lock(IO_sync::mutex);
	    std::cerr << "identity maps " << new_ls_img
		      << " to a better set than " << new_ls_org << " - true" << std::endl;
	  }
	  return true;
	}

	// now process all neutral non-identity switches mapping elements to elements:
	for (LabelSet::const_iterator preimg_iter = _non_trivial_colsetvec.at(i).begin();
	     preimg_iter != _non_trivial_colsetvec.at(i).end();
	     ++preimg_iter) {
	  const parameter_type j(*preimg_iter);
	  if (!ls_img.contains(j)) {

	    // we first have to make sure that ls_img hits
	    // the current extremal element elem of ls_org:
	    continue;
	  }
	  const LabelSet new_ls_img((ls_img - j).permute(this->at(i).at(j)));
	  if (not_canonicalPR(i + 1, new_ls_org, new_ls_img)) {
	    if (CommandlineOptions::debug()) {
	      std::lock_guard<std::mutex> lock(IO_sync::mutex);
	      std::cerr << "original set " << ls_org << " contains " << elem << std::endl;
	      std::cerr << "mapped set   " << ls_img << " contains " << elem << std::endl;
	      std::cerr << "G[" << i + 1 << "] maps " << new_ls_img
			<< " to a better set than " << new_ls_org << " - true" << std::endl;
	    }
	    return true;
	  }
	}
	if (CommandlineOptions::debug()) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << "original set " << ls_org << " contains " << elem << std::endl;
	  std::cerr << "mapped set   " << ls_img << " contains " << elem << std::endl;
	  std::cerr << "G[" << i + 1 << "] does not map " << new_ls_img
		    << " to a better set than " << new_ls_org << " - false" << std::endl;
	}
	return false;
      }
    }
  }  

  // extract those switches that may stabilize a LabelSet:
  template <comparison_mode_type mode>
  switch_selection_type SwitchTable<mode>::stabilizing_switchptrs(const parameter_type i,
								  const LabelSet&      ls_org,
								  const LabelSet&      ls_img) const {
    switch_selection_type result;
    result.reserve(_non_trivial_colsetvec.at(i).card());
    const parameter_type elem(_elem(i));
    if (_non_trivial_rowset.contains(i)) {
      for (LabelSet::const_iterator coliter = _non_trivial_colsetvec.at(i).begin();
	   coliter != _non_trivial_colsetvec.at(i).end();
	   ++coliter) {
	const parameter_type j(*coliter);
	if (ls_org.contains(elem) && !ls_img.contains(j)) {

	  // future switches can never push elem into ls_img again,
	  // so ls_img cannot be mapped to ls_org containing elem:
	  continue;
	}
	if (!ls_org.contains(elem) && ls_img.contains(j)) {

	  // future switches can never push elem out of ls_img again,
	  // so ls_img cannot be mapped to ls_org not containing elem:
	  continue;
	}
	result.emplace_back(&this->at(i).at(j));
      }
    }
    return result;
  }

  template <comparison_mode_type mode>
  size_type SwitchTable<mode>::count_stabilizing_switchproducts(const parameter_type i,
								const LabelSet&      ls_org,
								const LabelSet&      ls_img) const {
    if (empty()) {

      // count the identity:
      return 1UL;
    }
    if (i > _non_trivial_rowset.max_elem()) {

      // count the identity:
      return 1UL;
    }

    // count the identity first:
    size_type result = 1UL;    
    const parameter_type elem(_elem(i));

    // handle the identity individually without bogus mapping (the identity alone was counted already):
    if (!ls_org.contains(elem) && !ls_img.contains(elem)) {

      // elem is already missing in both ls_img and ls_org, which cannot be changed
      // by the remaining switches, since they fix elem:
      result += count_stabilizing_switchproducts(i + 1, ls_org, ls_img) - 1;
    }
    const LabelSet new_ls_org(ls_org - elem);
    LabelSet new_ls_img(ls_img - elem);
    if (ls_org.contains(elem) && ls_img.contains(elem)) { 

      // elem is already in both ls_img and ls_org, which cannot be changed
      // by the remaining switches, since they fix elem:
      result += count_stabilizing_switchproducts(i + 1, new_ls_org, new_ls_img) - 1;
    }

    if (_non_trivial_rowset.contains(i)) {
      for (LabelSet::const_iterator iter = _non_trivial_colsetvec.at(i).begin();
	   iter != _non_trivial_colsetvec.at(i).end();
	   ++iter) {
	const parameter_type j(*iter);
	if (ls_org.contains(elem) && !ls_img.contains(j)) {

	  // future switches can never push elem into ls_img again,
	  // so ls_img cannot be mapped using this switch
	  // to ls_org containing elem:
	  continue;
	}
	if (!ls_org.contains(elem) && ls_img.contains(j)) {

	  // future switches can never push elem out of ls_img again,
	  // so ls_img cannot be mapped using this switch
	  // to ls_org not containing elem:
	  continue;
	}
	if (this->at(i).at(j).maps(ls_img, ls_org)) {
	  
	  // found a stabilizing switch product:
	  ++result;
	  
	  // recursively extend the product, where we are already back at ls_org:
	  result += count_stabilizing_switchproducts(i + 1, new_ls_org, new_ls_org) - 1;
	}
	else {
	  
	  // map ls_img - j to new_ls_img:
	  new_ls_img = (ls_img - j).permute(this->at(i).at(j));
	  
	  // recursively extend the product (the identity was counted already):
	  result += count_stabilizing_switchproducts(i + 1, new_ls_org, new_ls_img) - 1;
	}
      }
    }
    return result;
  }

  template <comparison_mode_type mode>
  bool SwitchTable<mode>::lex_decreases(const LabelSet& ls) const {
    const bool result(not_canonicalPR(0, ls, ls));
    if (mode != lexmin_mode) {
      std::cerr << "SwitchTable::lex_decreases(...): wrong mode - exiting." << std::endl;
      exit(1);
    }
    if (CommandlineOptions::debug()) {
      if (result != not_canonical(0, ls, ls)) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "SwitchTable<mode>::lex_decreases(const LabelSet& ls) const: "
		  << "incompatible results of not_canonicalPR and not_canonical - exiting." << std::endl;
	std::cerr << "switch table:\n" << *this << std::endl;
	exit(1);
      }
    }
    return result;
  }

  template <comparison_mode_type mode>
  bool SwitchTable<mode>::colex_increases(const LabelSet& ls) const {
    const bool result(not_canonicalPR(0, ls, ls));
    if (mode != colexmax_mode) {
      std::cerr << "SwitchTable::colex_increases(...): wrong mode - exiting." << std::endl;
      exit(1);
    }
    if (CommandlineOptions::debug()) {
      if (result != not_canonical(0, ls, ls)) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "SwitchTable<mode>::colex_increases(const LabelSet& ls) const: "
		  << "incompatible results of not_canonicalPR and not_canonical - exiting." << std::endl;
	std::cerr << "switch table:\n" << *this << std::endl;
	exit(1);
      }
    }
    return result;
  }
  
  // stream input/output:
  template <comparison_mode_type mode>
  inline std::istream& SwitchTable<mode>::read(std::istream& ist) {
    std::cerr << "SwitchTable::read(std::istream&): reading pointers not supported - exiting" << std::endl;
    exit(1);
  }
  
  template <comparison_mode_type streammode>
  inline std::istream& operator>>(std::istream& ist, SwitchTable<streammode>& st) {
    return st.read(ist);
  }
  
  template <comparison_mode_type mode>
  inline std::ostream& SwitchTable<mode>::write(std::ostream& ost) const {
    for (LabelSet::const_iterator rowiter = _non_trivial_rowset.begin();
	 rowiter != _non_trivial_rowset.end();
	 ++rowiter) {
      ost << "row " << *rowiter << ":";
      for (LabelSet::const_iterator coliter = _non_trivial_colsetvec.at(*rowiter).begin();
	   coliter != _non_trivial_colsetvec.at(*rowiter).end();
	   ++coliter) {
	ost << " " << *coliter << " -> " << this->at(*rowiter).at(*coliter);
      }
      ost << '\n';
    }
    return ost;
  }
  
  template <comparison_mode_type streammode>
  inline std::ostream& operator<<(std::ostream& ost, const SwitchTable<streammode>& st) {
    return st.write(ost);
  }
  
  // internal methods:
  template <>
  inline parameter_type SwitchTable<lexmin_mode>::_elem(const parameter_type i) const {
    return i;
  }

  template <>
  inline parameter_type SwitchTable<colexmax_mode>::_elem(const parameter_type i) const {
    return _n - i - 1;
  }

  template <>
  inline parameter_type SwitchTable<lexmin_mode>::_mincol(const parameter_type elem) const {
    return elem + 1;
  }

  template <>
  inline parameter_type SwitchTable<colexmax_mode>::_mincol(const parameter_type elem) const {
    return 0;
  }

  template <>
  inline parameter_type SwitchTable<lexmin_mode>::_maxcol(const parameter_type elem) const {
    return _n;
  }

  template <>
  inline parameter_type SwitchTable<colexmax_mode>::_maxcol(const parameter_type elem) const {
    return elem;
  }

  template <>
  inline bool SwitchTable<lexmin_mode>::_compare_subsets(const LabelSet& ls1,
							 const LabelSet& ls2) const {
    return ls1.lexsmaller(ls2);
  }

  template <>
  inline bool SwitchTable<colexmax_mode>::_compare_subsets(const LabelSet& ls1,
							   const LabelSet& ls2) const {
    return ls1.colexgreater(ls2);
  }

  template <comparison_mode_type mode>
  bool SwitchTable<mode>::_add_symmetry(const Symmetry& sym) {
    for (parameter_type i = 0; i < _n; ++i) {
      const parameter_type elem = _elem(i);
      if (sym(elem) != elem) {
	
	// we are in row elem of the switch table,
	// and the column is the preimage of elem:
	for (parameter_type j = _mincol(elem); j < _maxcol(elem); ++j) {
	  if ((sym(j) == elem)
	      && !_non_trivial_colsetvec.at(i).contains(j)) {
	    if (CommandlineOptions::debug()) {
	      std::cerr << "adding switch " << sym << " to cell (" << i << "," << j << ") of switch table" << std::endl;
	    }
	    this->at(i).at(j) = sym;
	    _non_trivial_rowset += i;
	    _non_trivial_colsetvec.at(i) += j;
	    return true;
	  }
	}
	return false;
      }
    }
    return false;
  }

}; // namespace topcom

#endif

// eof SwitchTable.hh
